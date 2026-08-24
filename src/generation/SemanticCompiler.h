// SemanticCompiler.h
#ifndef SM64_THE_MUTANT_LIMINALITY_SEMANTICCOMPILER_H
#define SM64_THE_MUTANT_LIMINALITY_SEMANTICCOMPILER_H
#include "TriangleTypes.h"
#include "TriangleCollection.h"
#include "TriangleEdgeSplitSolver.h"
#include "SemanticEdge.h"
#include "VertexCollection.h"

#include <cmath>
#include <cstdint>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>

class SemanticCompiler {
    static std::deque<SemanticRelations> storage;

    static uint64_t edgeKey(int a, int b) {
        uint32_t lo = static_cast<uint32_t>(std::min(a, b));
        uint32_t hi = static_cast<uint32_t>(std::max(a, b));
        return (static_cast<uint64_t>(hi) << 32) | lo;
    }

    // Mismo umbral angular que TriangleEdgeSplitSolver::classifyEdge (~5°),
    // pero esta NO es esa función: BaseCastle::baseTriangles vive en un
    // arreglo estático que nunca pasa por TriangleCollection::createTriangle,
    // así que AbstractVertex::connectingTriangles está vacío para sus vértices
    // y classifyEdge no tiene con qué encontrar vecinos. Se reimplementa la
    // búsqueda de vecinos sobre el arreglo fijo de 12 triángulos.
    //
    // Divergencia deliberada respecto a classifyEdge: además de normal,
    // exige mismo frontType/backType, siguiendo la definición de "arista
    // estructural" del documento de principios (21/08) en vez de la de
    // TriangleEdgeSplitSolver (que solo le importa normal, porque resuelve
    // otro problema).
    static constexpr float kCoplanarDotThreshold = 0.996f;

    static EdgeStrength classifyBaseCastleEdge(
        int va, int vb,
        const std::unordered_map<uint64_t, std::vector<int>>& edgeOwners)
    {
        auto it = edgeOwners.find(edgeKey(va, vb));
        if (it == edgeOwners.end() || it->second.size() == 1) {
            // Borde libre: sin vecino, no hay con qué fusionar -> Strong.
            return EdgeStrength::Strong;
        }
        if (it->second.size() > 2) {
            // No-manifold: 3+ triángulos comparten la arista -> Strong.
            return EdgeStrength::Strong;
        }

        const AbstractTriangle& tA = BaseCastle::baseTriangles[it->second[0]];
        const AbstractTriangle& tB = BaseCastle::baseTriangles[it->second[1]];

        bool sameNormal = tA.getNormal().dot(tB.getNormal()) >= kCoplanarDotThreshold;
        bool sameMaterial = tA.getFrontType() == tB.getFrontType() &&
                             tA.getBackType()  == tB.getBackType();

        return (sameNormal && sameMaterial) ? EdgeStrength::Weak : EdgeStrength::Strong;
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

    // SemanticCompiler.h — reemplaza el segundo pase del compile()

    // NUEVO: recolecta todos los TriangleFaceType (front + back, sin None,
    // sin duplicados) de TODOS los dueños de una arista. A diferencia del
    // código anterior, esto ya no depende de "por cuál triángulo llegué",
    // es una propiedad simétrica de la arista misma.
    static std::vector<TriangleFaceType> collectEdgeTriangleTypes(
        uint64_t key,
        const std::unordered_map<uint64_t, std::vector<int>>& edgeOwners)
    {
        std::vector<TriangleFaceType> types;
        auto it = edgeOwners.find(key);
        if (it == edgeOwners.end()) return types;

        for (int ownerIdx : it->second) {
            const AbstractTriangle& tri = BaseCastle::baseTriangles[ownerIdx];

            TriangleFaceType front = tri.getFrontType();
            if (front != TriangleFaceType::None &&
                std::find(types.begin(), types.end(), front) == types.end()) {
                types.push_back(front);
            }

            TriangleFaceType back = tri.getBackType();
            if (back != TriangleFaceType::None &&
                std::find(types.begin(), types.end(), back) == types.end()) {
                types.push_back(back);
            }
        }
        return types;
    }

public:
    static std::unordered_map<int, int> compile(std::vector<SemanticEdge>& edges, void *heatmap = nullptr) {
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

        std::unordered_map<uint64_t, std::vector<int>> edgeOwners;
        for (int t = 0; t < triCount; ++t) {
            AbstractTriangle* tri = &BaseCastle::baseTriangles[t];
            for (int i = 0; i < 3; ++i) {
                int va = tri->getVertex(i);
                int vb = tri->getVertex((i + 1) % 3);
                edgeOwners[edgeKey(va, vb)].push_back(t);
            }
        }

        for (int t = 0; t < triCount; ++t) {
            AbstractTriangle* tri = &BaseCastle::baseTriangles[t];

            for (int i = 0; i < 3; ++i) {
                int va = tri->getVertex(i);
                int vb = tri->getVertex((i + 1) % 3);

                uint64_t key = edgeKey(va, vb);
                if (visitedEdges.count(key)) continue;
                visitedEdges.insert(key);

                if (classifyBaseCastleEdge(va, vb, edgeOwners) == EdgeStrength::Weak) {
                    ensureRelations(va);
                    ensureRelations(vb);
                    continue;
                }

                int idxA = ensureRelations(va);
                int idxB = ensureRelations(vb);

                RelativeTransform aToB = computeTransform(va, vb);
                RelativeTransform bToA = computeTransform(vb, va);

                // Ambas direcciones comparten la misma lista de tipos:
                // es una propiedad de la arista, no de por dónde se recorre.
                std::vector<TriangleFaceType> sharedTypes = collectEdgeTriangleTypes(key, edgeOwners);

                edges.push_back(SemanticEdge{ idxB, sharedTypes, aToB });
                storage[idxA].addEdge(edges.size() - 1);
                edges.push_back(SemanticEdge{ idxA, sharedTypes, bToA });
                storage[idxB].addEdge(edges.size() - 1);
            }
        }

        return relationsByVertex;
    }

    static SemanticRelations* get(int idx) {
        if (idx < 0 || idx >= static_cast<int>(storage.size())) return nullptr;
        return &storage[idx];
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_SEMANTICCOMPILER_H