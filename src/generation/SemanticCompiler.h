//
// Created by Luis Alvarez on 21/08/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_SEMANTICCOMPILER_H
#define SM64_THE_MUTANT_LIMINALITY_SEMANTICCOMPILER_H
#include "TriangleTypes.h"
#include "TriangleCollection.h"
#include "SemanticEdge.h"
#include "VertexCollection.h"

#include <cmath>
#include <cstdint>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class SemanticCompiler {
    // Storage con direcciones estables (mismo motivo que VertexCollection /
    // TriangleCollection usan deque): cada SemanticEdge guarda un puntero
    // crudo a un SemanticRelations vecino, y ese puntero no puede invalidarse
    // si el storage crece.
    static std::deque<SemanticRelations> storage;

    static uint64_t edgeKey(int a, int b) {
        uint32_t lo = static_cast<uint32_t>(std::min(a, b));
        uint32_t hi = static_cast<uint32_t>(std::max(a, b));
        return (static_cast<uint64_t>(hi) << 32) | lo;
    }

    static RelativeTransform computeTransform(int from, int to) {
        const Vector3& a = VertexCollection::getVertex(from)->position;
        const Vector3& b = VertexCollection::getVertex(to)->position;
        Vector3 delta = b - a;
        float distance = std::sqrt(static_cast<float>(delta.x) * (float)delta.x +
                                    static_cast<float>(delta.z) * (float)delta.z);
        float yaw = std::atan2(static_cast<float>(delta.z), static_cast<float>(delta.x));
        return RelativeTransform{ distance, yaw, delta.y };
    }

public:
    // Devuelve un mapa vertexId -> SemanticRelations*. Llamar de nuevo invalida
    // TODOS los punteros previos (storage.clear()) -- no guardes SemanticVertex
    // vivos a través de dos llamadas a compile().
    static std::unordered_map<int, SemanticRelations*> compile(void *heatmap = nullptr) {
        storage.clear();

        std::unordered_map<int, SemanticRelations*> relationsByVertex;
        std::unordered_set<uint64_t> visitedEdges;

        auto ensureRelations = [&](int vid) -> SemanticRelations* {
            auto it = relationsByVertex.find(vid);
            if (it != relationsByVertex.end()) return it->second;
            storage.emplace_back();
            SemanticRelations* rel = &storage.back();
            relationsByVertex[vid] = rel;
            return rel;
        };

        int triCount = 12;
        for (int t = 0; t < triCount; ++t) {
            AbstractTriangle* tri = &BaseCastle::baseTriangles[t];
            //if (!tri->isBuilt()) continue; // solo geometría final/definitiva se vuelve semántica

            for (int i = 0; i < 3; ++i) {
                int va = tri->getVertex(i);
                int vb = tri->getVertex((i + 1) % 3);

                uint64_t key = edgeKey(va, vb);
                if (visitedEdges.count(key)) continue;
                visitedEdges.insert(key);

                SemanticRelations* relA = ensureRelations(va);
                SemanticRelations* relB = ensureRelations(vb);

                RelativeTransform aToB = computeTransform(va, vb);
                RelativeTransform bToA = computeTransform(vb, va);

                relA->addEdge(SemanticEdge{ relB, { tri->getFrontType() }, aToB });
                relB->addEdge(SemanticEdge{ relA, { tri->getFrontType() }, bToA });
            }
        }

        return relationsByVertex;
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_SEMANTICCOMPILER_H