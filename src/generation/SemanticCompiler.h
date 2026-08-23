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
    // Devuelve un mapa vertexId (del castillo base) -> índice en `storage`.
    // Ese mapa SOLO sirve para el bootstrap (encontrar el índice semántico
    // del vértice raíz); a partir de ahí toda la navegación es índice->índice
    // vía SemanticEdge::relations, sin volver a tocar este mapa.
    // Llamar de nuevo invalida todos los índices previos (storage.clear()).
    static std::unordered_map<int, int> compile(void *heatmap = nullptr) {
        storage.clear();

        std::unordered_map<int, int> relationsByVertex;
        std::unordered_set<uint64_t> visitedEdges;

        auto ensureRelations = [&](int vid) -> int {
            auto it = relationsByVertex.find(vid);
            if (it != relationsByVertex.end()) return it->second;
            storage.emplace_back();
            int idx = static_cast<int>(storage.size()) - 1;
            relationsByVertex[vid] = idx;
            return idx;
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

                int idxA = ensureRelations(va);
                int idxB = ensureRelations(vb);

                RelativeTransform aToB = computeTransform(va, vb);
                RelativeTransform bToA = computeTransform(vb, va);

                storage[idxA].addEdge(SemanticEdge{ idxB, { tri->getFrontType() }, aToB });
                storage[idxB].addEdge(SemanticEdge{ idxA, { tri->getFrontType() }, bToA });
            }
        }

        return relationsByVertex;
    }

    // Único punto de resolución índice -> objeto, igual que
    // VertexCollection::getVertex / TriangleCollection::getTriangle.
    static SemanticRelations* get(int idx) {
        if (idx < 0 || idx >= static_cast<int>(storage.size())) return nullptr;
        return &storage[idx];
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_SEMANTICCOMPILER_H