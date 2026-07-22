//
// Created by Luis Alvarez on 19/07/2026.
//

#include "TriangleEdgeSplitSolver.h"
#include "AbstractTriangle.h"
#include "VertexCollection.h"
#include "TriangleCollection.h"

#include <cmath>
#include <iostream>

std::unordered_map<TemporalVertexRegistry::Key, TemporalVertexRegistry::Entry, TemporalVertexRegistry::KeyHash>
    TemporalVertexRegistry::table;

TemporalVertexRegistry::Key TemporalVertexRegistry::quantize(const Vector3& p, float epsilon) {
    return Key{
        static_cast<int>(std::round(static_cast<float>(p.x) / epsilon)),
        static_cast<int>(std::round(static_cast<float>(p.y) / epsilon)),
        static_cast<int>(std::round(static_cast<float>(p.z) / epsilon))
    };
}

int TemporalVertexRegistry::getOrCreate(const Vector3& p, bool asStrong, float epsilon) {
    Key k = quantize(p, epsilon);
    auto it = table.find(k);
    if (it != table.end()) {
        if (asStrong) it->second.strong = true;
        return it->second.vertexHandle;
    }
    int handle = VertexCollection::createVertex(p);
    table[k] = Entry{ handle, asStrong };
    return handle;
}

bool TriangleEdgeSplitSolver::segmentCrossesTriangleInterior(
    const Vector3& p0, const Vector3& p1, int triId, Vector3& outPoint
) {
    AbstractTriangle* tri = TriangleCollection::getTriangle(triId);
    const Vector3& a = VertexCollection::getVertex(tri->getVertex(0))->position;
    const Vector3& b = VertexCollection::getVertex(tri->getVertex(1))->position;
    const Vector3& c = VertexCollection::getVertex(tri->getVertex(2))->position;
    const Vector3& n = tri->getNormal();

    float d0 = (p0 - a).dot(n);
    float d1 = (p1 - a).dot(n);

    // Ambos extremos del mismo lado del plano -> no hay cruce.
    if ((d0 > kWorldEpsilon && d1 > kWorldEpsilon) || (d0 < -kWorldEpsilon && d1 < -kWorldEpsilon)) {
        return false;
    }
    // Segmento (casi) paralelo al plano: caso degenerado, fuera de alcance mínimo.
    if (std::abs(d0 - d1) < 1e-6f) return false;

    float t = d0 / (d0 - d1);
    if (t < 0.0f || t > 1.0f) return false; // el cruce cae fuera del segmento real

    Vector3 p = p0 + (p1 - p0) * t;

    // Coordenadas baricéntricas de p respecto a (a,b,c)
    Vector3 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = v0.dot(v0), d01 = v0.dot(v1), d11 = v1.dot(v1);
    float d20 = v2.dot(v0), d21 = v2.dot(v1);
    float denom = d00 * d11 - d01 * d01;
    if (std::abs(denom) < 1e-9f) return false; // triángulo degenerado

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    // Interior ESTRICTO: si cae sobre un edge propio de triId (u, v o w ~ 0),
    // lo descartamos aquí a propósito -> ese es el caso "split en 2", fuera
    // de alcance de esta versión mínima.
    if (u > kBaryMargin && v > kBaryMargin && w > kBaryMargin) {
        outPoint = p;
        return true;
    }
    return false;
}

EdgeStrength TriangleEdgeSplitSolver::classifyEdge(int ownerTri, int v1, int v2, int& outNeighborTri) {
    outNeighborTri = -1;

    const auto& trisV1 = VertexCollection::getVertex(v1)->connectingTriangles;
    const auto& trisV2 = VertexCollection::getVertex(v2)->connectingTriangles;

    std::vector<int> sharedOwners;
    for (int t : trisV1) {
        if (t == ownerTri) continue;
        if (TriangleCollection::getTriangle(t)->isBuilt()) continue; // <-- filtra muertos/superseded
        for (int t2 : trisV2) {
            if (t == t2) { sharedOwners.push_back(t); break; }
        }
    }

    if (sharedOwners.empty()) {
        // Edge de borde libre: no hay con quién fusionar, lo tratamos Strong
        // (no se puede volver "weak" un edge sin vecino).
        return EdgeStrength::Strong;
    }

    if (sharedOwners.size() > 1) {
        // Non-manifold: compartido por 3+ triángulos -> Strong por definición.
        outNeighborTri = sharedOwners[0];
        return EdgeStrength::Strong;
    }

    // Exactamente un vecino: Weak solo si además comparte normal.
    int neighbor = sharedOwners[0];
    AbstractTriangle* ownerT = TriangleCollection::getTriangle(ownerTri);
    AbstractTriangle* neighborT = TriangleCollection::getTriangle(neighbor);

    float normalDot = ownerT->getNormal().dot(neighborT->getNormal());
    outNeighborTri = neighbor;

    // Umbral generoso: ~5 grados de tolerancia antes de considerarla "distinta".
    constexpr float kNormalDotThreshold = 0.996f;
    if (normalDot >= kNormalDotThreshold) {
        return EdgeStrength::Weak;
    }
    return EdgeStrength::Strong;
}

std::array<int, 3> TriangleEdgeSplitSolver::splitTriangleAtInteriorPoint(int triId, int newVertex) {
    AbstractTriangle* tri = TriangleCollection::getTriangle(triId);

    int v0 = tri->getVertex(0);
    int v1 = tri->getVertex(1);
    int v2 = tri->getVertex(2);

    // Si ya existía un vértice temporal en este punto (creado por el lado Weak
    // de triB en un paso previo), lo reusamos y lo promovemos a fuerte.
    // Esto ES la "redirección de referencias" del pseudocódigo original: como
    // nunca cambiamos el handle, no hace falta recorrer y reescribir nada.

    tri->markSuperseded();

    // Fan desde newVertex, preservando el tipo concreto y el orden de winding
    // (para no invertir la normal original).
    int t0 = tri->cloneWithVertices(v0, v1, newVertex);
    int t1 = tri->cloneWithVertices(v1, v2, newVertex);
    int t2 = tri->cloneWithVertices(v2, v0, newVertex);

    return { t0, t1, t2 };
}