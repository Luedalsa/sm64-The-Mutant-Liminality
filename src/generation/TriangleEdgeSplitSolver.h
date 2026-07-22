//
// Created by Luis Alvarez on 19/07/2026.
//
// TriangleEdgeSplitSolver
//
// Versión MÍNIMA del algoritmo de resolución local para el caso:
// "un triángulo opuesto (triB) intersecta a este triángulo (triA)
//  a través de exactamente UN edge de triB, en un único punto interior de triA".
//
// Cubre:
//   - Caso Strong: split de triA en 3 (fan) usando el punto de intersección.
//     Incluye promoción de vértice temporal -> fuerte (reutilización de handle).
//   - Caso Weak:   registro de vértice temporal + flip del cuadrilátero
//                  convexo formado por el edge weak de triB y su único vecino.
//
// Explícitamente FUERA de alcance en esta versión (ver comentarios TODO):
//   - Más de un edge de triB cruzando triA (penetración completa).
//   - Punto de intersección sobre un edge propio de triA (split en 2, no en 3).
//   - Caso Weak cóncavo (retriangulación con vértice temporal como ancla).
//   - Redistribución de promesas (promise) al hacer split/flip.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_TRIANGLEEDGESPLITSOLVER_H
#define SM64_THE_MUTANT_LIMINALITY_TRIANGLEEDGESPLITSOLVER_H

#include "Primitives.h"
#include <array>
#include <unordered_map>

enum class EdgeStrength {
    Strong,
    Weak
};

class TemporalVertexRegistry {
public:
    struct Key {
        int x, y, z;
        bool operator==(const Key& o) const { return x == o.x && y == o.y && z == o.z; }
    };
    struct KeyHash {
        size_t operator()(const Key& k) const {
            size_t h = std::hash<int>()(k.x);
            h ^= std::hash<int>()(k.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<int>()(k.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
    struct Entry {
        int vertexHandle;
        bool strong;
    };

private:
    static std::unordered_map<Key, Entry, KeyHash> table;
    static Key quantize(const Vector3& p, float epsilon);

public:
    static int getOrCreate(const Vector3& p, bool asStrong, float epsilon = 1.0f);

    static void clear() { table.clear(); }
};

class TriangleEdgeSplitSolver {
public:
    static bool resolveSingleEdgeIntersection(int triA, int triB);
  bool flipWeaks(int triA, int triB);

//private:
    static constexpr float kWorldEpsilon = 1.0f;
    static constexpr float kBaryMargin   = 0.001f;

    static bool segmentCrossesTriangleInterior(const Vector3& p0, const Vector3& p1, int triId, Vector3& outPoint);

    static EdgeStrength classifyEdge(int ownerTri, int v1, int v2, int& outNeighborTri);

    static std::array<int, 3> splitTriangleAtInteriorPoint(int triId, int newVertex);
};

#endif // SM64_THE_MUTANT_LIMINALITY_TRIANGLEEDGESPLITSOLVER_H
