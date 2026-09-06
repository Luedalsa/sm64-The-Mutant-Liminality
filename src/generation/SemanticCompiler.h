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
        auto relationsByVertex = std::unordered_map<int, int>{  };
        storage.push_back(SemanticRelations{});
        int pillarA = storage.size() - 1;
        relationsByVertex[pillarA] = pillarA;
        storage.push_back(SemanticRelations{});
        int pillarB = storage.size() - 1;
        relationsByVertex[pillarB] = pillarB;
        storage.push_back(SemanticRelations{});
        int pillarC = storage.size() - 1;
        relationsByVertex[pillarC] = pillarC;
        storage.push_back(SemanticRelations{});
        int pillarD = storage.size() - 1;
        relationsByVertex[pillarD] = pillarD;
        storage.push_back(SemanticRelations{});
        int pillarE = storage.size() - 1;
        relationsByVertex[pillarE] = pillarE;
        storage.push_back(SemanticRelations{});
        int pillarF = storage.size() - 1;
        relationsByVertex[pillarF] = pillarF;
        storage.push_back(SemanticRelations{});
        int pillarG = storage.size() - 1;
        relationsByVertex[pillarG] = pillarG;
        storage.push_back(SemanticRelations{});
        int pillarH = storage.size() - 1;
        relationsByVertex[pillarH] = pillarH;
        storage.push_back(SemanticRelations{});
        int pillarI = storage.size() - 1;
        relationsByVertex[pillarI] = pillarI;
        storage.push_back(SemanticRelations{});
        int WallA = storage.size() - 1;
        relationsByVertex[WallA] = WallA;
        storage.push_back(SemanticRelations{});
        int WallB = storage.size() - 1;
        relationsByVertex[WallB] = WallB;
        storage.push_back(SemanticRelations{});
        int diagonalA = storage.size() - 1;
        relationsByVertex[diagonalA] = diagonalA;
        storage.push_back(SemanticRelations{});
        int WallC = storage.size() - 1;
        relationsByVertex[WallC] = WallC;
        storage.push_back(SemanticRelations{});
        int WallD = storage.size() - 1;
        relationsByVertex[WallD] = WallD;
        storage.push_back(SemanticRelations{});
        int diagonalB = storage.size() - 1;
        relationsByVertex[diagonalB] = diagonalB;
        storage.push_back(SemanticRelations{});
        int WallE = storage.size() - 1;
        relationsByVertex[WallE] = WallE;
        storage.push_back(SemanticRelations{});
        int pillarJ = storage.size() - 1;
        relationsByVertex[pillarJ] = pillarJ;
        storage.push_back(SemanticRelations{});
        int pillarK = storage.size() - 1;
        relationsByVertex[pillarK] = pillarK;
        storage.push_back(SemanticRelations{});
        int pillarL = storage.size() - 1;
        relationsByVertex[pillarL] = pillarL;
        storage.push_back(SemanticRelations{});
        int WallF = storage.size() - 1;
        relationsByVertex[WallF] = WallF;
        storage.push_back(SemanticRelations{});
        int diagonalC = storage.size() - 1;
        relationsByVertex[diagonalC] = diagonalC;
        storage.push_back(SemanticRelations{});
        int WallG = storage.size() - 1;
        relationsByVertex[WallG] = WallG;
        storage.push_back(SemanticRelations{});
        int diagonalD = storage.size() - 1;
        relationsByVertex[diagonalD] = diagonalD;
        storage.push_back(SemanticRelations{});
        int WallH = storage.size() - 1;
        relationsByVertex[WallH] = WallH;
        storage.push_back(SemanticRelations{});
        int diagonalE = storage.size() - 1;
        relationsByVertex[diagonalE] = diagonalE;
        storage.push_back(SemanticRelations{});
        int WallI = storage.size() - 1;
        relationsByVertex[WallI] = WallI;
        storage.push_back(SemanticRelations{});
        int diagonalF = storage.size() - 1;
        relationsByVertex[diagonalF] = diagonalF;
        storage.push_back(SemanticRelations{});
        int WallJ = storage.size() - 1;
        relationsByVertex[WallJ] = WallJ;
        storage.push_back(SemanticRelations{});
        int WallK = storage.size() - 1;
        relationsByVertex[WallK] = WallK;
        storage.push_back(SemanticRelations{});
        static std::vector<TriangleFaceType> sharedTypes = {
            TriangleFaceType::InsideWallFirstFloor,
            TriangleFaceType::InsideRoofFirstFloor,
            TriangleFaceType::CheckerboardFloor
        };
        storage[pillarA].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ pillarB, sharedTypes, RelativeTransform{ 51.0f, 3.141592f/2.0f, 0.0f } });
        storage[pillarB].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ pillarC, sharedTypes, RelativeTransform{ 72.83f, -3.141592f/4.0f, 0.0f } });
        storage[pillarC].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ WallA, sharedTypes, RelativeTransform{ 51.83f, -3.141592f/4.0f, 0.0f } });
        int pilA = edges.size();
        edges.push_back(SemanticEdge{ pillarD, sharedTypes, RelativeTransform{ 2251.0f, 3.141592f/2.0f, 0.0f } });
        int pilB = edges.size();
        edges.push_back(SemanticEdge{ pillarA, sharedTypes, RelativeTransform{ 410.0f, 3.141592f/2.0f, 0.0f } });
        storage[WallA].addEdge(pilA, 10);
        //storage[WallA].addEdge(pilB, 1);
        storage[pillarD].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ pillarE, sharedTypes, RelativeTransform{ 51.0f, 3.141592f/2.0f, 0.0f } });
        storage[pillarE].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ pillarF, sharedTypes, RelativeTransform{ 72.83f, -3.141592f/4.0f, 0.0f } });
        storage[pillarF].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ WallB, sharedTypes, RelativeTransform{ 51.83f, -3.141592f/4.0f, 0.0f } });
        //storage[WallB].addEdge(pilA, 1);
        //storage[WallB].addEdge(pilB, 10);
        storage[WallB].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ pillarG, sharedTypes, RelativeTransform{ 410.0f, 3.141592f/2.0f, 0.0f } });
        storage[pillarG].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ pillarH, sharedTypes, RelativeTransform{ 51.83f, 3.141592f/2.0f, 0.0f } });
        storage[pillarH].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ pillarI, sharedTypes, RelativeTransform{ 72.83, -3.141592f/4.0f, 0.0f } });
        storage[pillarI].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ WallC, sharedTypes, RelativeTransform{ 51.83f, -3.141592f/4.0f, 0.0f } });
        storage[WallC].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ diagonalA, sharedTypes, RelativeTransform{ 1637.0f, 3.141592f/2.0f, 0.0f } });
        storage[diagonalA].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ WallD, sharedTypes, RelativeTransform{ 144.959f, -3.141592f/4.0f, 0.0f } });
        storage[WallD].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ diagonalB, sharedTypes, RelativeTransform{ 1945.0f, -3.141592f/4.0f, 0.0f } });
        storage[diagonalB].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ WallE, sharedTypes, RelativeTransform{ 144.959f, -3.141592f/4.0f, 0.0f } });
        storage[WallE].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ pillarJ, sharedTypes, RelativeTransform{ 1228.0f, -3.141592f/4.0f, 0.0f } });
        storage[pillarJ].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ pillarK, sharedTypes, RelativeTransform{ 51.83f, 3.141592f/2.0f, 0.0f } });
        storage[pillarK].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ pillarL, sharedTypes, RelativeTransform{ 72.83f, -3.141592f/4.0f, 0.0f } });
        storage[pillarL].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ WallF, sharedTypes, RelativeTransform{ 51.83f, -3.141592f/4.0f, 0.0f } });
        storage[WallF].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ diagonalC, sharedTypes, RelativeTransform{ 205.0f, 3.141592f/2.0f, 0.0f } });
        storage[diagonalC].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ WallG, sharedTypes, RelativeTransform{ 144.959f, -3.141592f/4.0f, 0.0f } });
        storage[WallG].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ diagonalD, sharedTypes, RelativeTransform{ 820.0f, -3.141592f/4.0f, 0.0f } });
        storage[diagonalD].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ WallH, sharedTypes, RelativeTransform{ 144.959f, -3.141592f/4.0f, 0.0f } });
        storage[WallH].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ diagonalE, sharedTypes, RelativeTransform{ 717.0f, -3.141592f/4.0f, 0.0f } });
        storage[diagonalE].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ WallI, sharedTypes, RelativeTransform{ 144.959f, -3.141592f/4.0f, 0.0f } });
        storage[WallI].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ WallJ, sharedTypes, RelativeTransform{ 1639.0f, -3.141592f/4.0f, 0.0f } });
        storage[WallJ].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ WallK, sharedTypes, RelativeTransform{ 625.0f, 3.141592f/2.0f, 0.0f } });
        storage[WallK].addEdge(edges.size(), 1);
        edges.push_back(SemanticEdge{ pillarA, sharedTypes, RelativeTransform{ 2253.0f, 3.141592f/2.0f, 0.0f } });

        return relationsByVertex;
    }

    static SemanticRelations* get(int idx) {
        if (idx < 0 || idx >= static_cast<int>(storage.size())) return nullptr;
        return &storage[idx];
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_SEMANTICCOMPILER_H