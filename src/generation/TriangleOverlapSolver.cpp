// src/generation/TriangleOverlapSolver.cpp
//
// Created by Luis Alvarez on 09/07/2026.
// Reescrito 19/07/2026 para integrar TriangleEdgeSplitSolver.
// Reescrito 04/08/2026: ya no crea vértices/triángulos, los recibe hechos.
//

#include "TriangleOverlapSolver.h"

#include "AbstractTriangle.h"
#include "DebugExport.h"
#include "TriangleCollection.h"
#include "TriangleEdgeSplitSolver.h"

#include <queue>

namespace {

struct AABB { Vector3 lo, hi; };

AABB triangleBounds(int triId, float margin = 1.0f) {
    AbstractTriangle* tri = TriangleCollection::getTriangle(triId);
    Vector3 a = VertexCollection::getVertex(tri->getVertex(0))->position;
    Vector3 b = VertexCollection::getVertex(tri->getVertex(1))->position;
    Vector3 c = VertexCollection::getVertex(tri->getVertex(2))->position;

    AABB box;
    box.lo = Vector3(
        std::min(a.x, std::min(b.x, c.x)) - margin,
        std::min(a.y, std::min(b.y, c.y)) - margin,
        std::min(a.z, std::min(b.z, c.z)) - margin
    );
    box.hi = Vector3(
        std::max(a.x, std::max(b.x, c.x)) + margin,
        std::max(a.y, std::max(b.y, c.y)) + margin,
        std::max(a.z, std::max(b.z, c.z)) + margin
    );
    return box;
}

bool aabbOverlap(const AABB& a, const AABB& b) {
    return (a.lo.x <= b.hi.x && a.hi.x >= b.lo.x) &&
           (a.lo.y <= b.hi.y && a.hi.y >= b.lo.y) &&
           (a.lo.z <= b.hi.z && a.hi.z >= b.lo.z);
}

} // namespace


namespace {

// ── Detección pura: no muta nada, solo reporta qué edges cruzan ──
struct EdgeCrossing {
    int va, vb;
    EdgeStrength strength;
    Vector3 point;
    int hitEdgeIndex = -1; // -1 = interior de interiorTriId; 0..2 = edge propio
};

std::vector<EdgeCrossing> detectCrossings(int edgeOwnerId, int interiorTriId) {
    std::vector<EdgeCrossing> crossings;
    AbstractTriangle* edgeOwner = TriangleCollection::getTriangle(edgeOwnerId);

    for (int i = 0; i < 3; ++i) {
        int va = edgeOwner->getVertex(i);
        int vb = edgeOwner->getVertex((i + 1) % 3);
        int neighbor = -1;
        EdgeStrength strength = TriangleEdgeSplitSolver::classifyEdge(edgeOwnerId, va, vb, neighbor);

        const Vector3& p0 = VertexCollection::getVertex(va)->position;
        const Vector3& p1 = VertexCollection::getVertex(vb)->position;
        Vector3 point;

        if (TriangleEdgeSplitSolver::segmentCrossesTriangleInterior(p0, p1, interiorTriId, point)) {
            crossings.push_back({va, vb, strength, point, -1});
            continue;
        }

        TriangleEdgeSplitSolver::EdgeIntersection edgeHit;
        if (TriangleEdgeSplitSolver::segmentCrossesTriangleEdge(p0, p1, interiorTriId, edgeHit)) {
            crossings.push_back({va, vb, strength, edgeHit.point, edgeHit.edgeIndex});
        }
    }
    return crossings;
}

void splitEdgeAcrossOwners(int va, int vb, int newVertex) {
    std::vector<int> owners;
    const auto& trisV1 = VertexCollection::getVertex(va)->connectingTriangles;
    const auto& trisV2 = VertexCollection::getVertex(vb)->connectingTriangles;

    for (int t : trisV1) {
        if (TriangleCollection::getTriangle(t)->isBuilt()) continue;
        for (int t2 : trisV2) {
            if (t == t2) { owners.push_back(t); break; }
        }
    }

    for (int ownerId : owners) {
        AbstractTriangle* owner = TriangleCollection::getTriangle(ownerId);
        if (owner->isBuilt()) continue;

        int opposite = -1, idxA = -1, idxB = -1;
        for (int i = 0; i < 3; ++i) {
            int v = owner->getVertex(i);
            if (v == va) idxA = i;
            else if (v == vb) idxB = i;
            else opposite = v;
        }
        if (opposite == -1 || idxA == -1 || idxB == -1) continue;

        owner->markSuperseded();

        if ((idxA + 1) % 3 == idxB) {
            owner->cloneWithVertices(va, newVertex, opposite);
            owner->cloneWithVertices(newVertex, vb, opposite);
        } else {
            owner->cloneWithVertices(vb, newVertex, opposite);
            owner->cloneWithVertices(newVertex, va, opposite);
        }
    }
}

void resolveStrongEdgeSplit(int va, int vb, const Vector3& point, int splitTargetTriId,
                             std::queue<int>& pending) {
    int beforeCount = TriangleCollection::getTriangleCount();

    int newVertex = TemporalVertexRegistry::getOrCreate(point, /*asStrong=*/true, 1.0f);
    TriangleEdgeSplitSolver::splitTriangleAtInteriorPoint(splitTargetTriId, newVertex);
    splitEdgeAcrossOwners(va, vb, newVertex);

    int afterCount = TriangleCollection::getTriangleCount();
    for (int newId = beforeCount; newId < afterCount; ++newId) {
        pending.push(newId);
    }
}

void resolveEdgeCrossingSplit(int triId, int edgeIndex, const Vector3& point, std::queue<int>& pending) {
    AbstractTriangle* tri = TriangleCollection::getTriangle(triId);
    int ea = tri->getVertex(edgeIndex);
    int eb = tri->getVertex((edgeIndex + 1) % 3);

    int newVertex = TemporalVertexRegistry::getOrCreate(point, /*asStrong=*/true, 1.0f);

    if (newVertex == ea || newVertex == eb) return;

    int beforeCount = TriangleCollection::getTriangleCount();
    splitEdgeAcrossOwners(ea, eb, newVertex);
    int afterCount = TriangleCollection::getTriangleCount();

    for (int newId = beforeCount; newId < afterCount; ++newId) {
        pending.push(newId);
    }
}

} // namespace

void TriangleOverlapSolver::resolve(const std::vector<int>& committed) {

    std::queue<int> pending;
    std::queue<std::array<int, 2>> sleepIsForTheWeak;
    for (int id : committed) pending.push(id);
while (!pending.empty()) {
    int triId = pending.front();
    pending.pop();

    AbstractTriangle* tri = TriangleCollection::getTriangle(triId);
    if (tri->isBuilt()) continue;

    AABB boxA = triangleBounds(triId);
    int total = TriangleCollection::getTriangleCount();

    for (int otherId = 0; otherId < total; ++otherId) {
        if (otherId == triId) continue;

        AbstractTriangle* other = TriangleCollection::getTriangle(otherId);
        if (other->isBuilt()) continue;
        if (!aabbOverlap(boxA, triangleBounds(otherId))) continue;

        auto crossingsDir1 = detectCrossings(otherId, triId);
        auto crossingsDir2 = detectCrossings(triId, otherId);

        // Los Weak solo se registran para la fase diferida. NO cortan la búsqueda.
        for (auto& c : crossingsDir1)
            if (c.hitEdgeIndex == -1 && c.strength == EdgeStrength::Weak)
                sleepIsForTheWeak.push({c.va, c.vb});
        for (auto& c : crossingsDir2)
            if (c.hitEdgeIndex == -1 && c.strength == EdgeStrength::Weak)
                sleepIsForTheWeak.push({c.va, c.vb});

        // Buscamos un Strong real. Solo esto justifica cortar el loop de otherId,
        // porque solo esto deja a triId superseded (y por lo tanto "terminado").
        bool resolvedStrong = false;

        for (auto& c : crossingsDir1) {
            if (c.strength != EdgeStrength::Strong) continue;
            if (c.hitEdgeIndex != -1) {
                resolveEdgeCrossingSplit(triId, c.hitEdgeIndex, c.point, pending);
            } else {
                resolveStrongEdgeSplit(c.va, c.vb, c.point, triId, pending);
            }
            resolvedStrong = true;
            break;
        }

        if (!resolvedStrong) {
            for (auto& c : crossingsDir2) {
                if (c.strength != EdgeStrength::Strong) continue;
                if (c.hitEdgeIndex != -1) {
                    resolveEdgeCrossingSplit(otherId, c.hitEdgeIndex, c.point, pending);
                } else {
                    resolveStrongEdgeSplit(c.va, c.vb, c.point, otherId, pending);
                }
                resolvedStrong = true;
                break;
            }
        }

        if (resolvedStrong) break; // triId cambió, se sigue vía sus hijos en pending
        // si no hubo Strong (solo Weak o nada), seguimos probando el resto de otherId
    }
}

    while (!sleepIsForTheWeak.empty()) {
        auto edge = sleepIsForTheWeak.front();
        sleepIsForTheWeak.pop();

        std::vector<int> owners;
        const auto& trisV1 = VertexCollection::getVertex(edge[0])->connectingTriangles;
        const auto& trisV2 = VertexCollection::getVertex(edge[1])->connectingTriangles;

        for (int t : trisV1) {
            if (TriangleCollection::getTriangle(t)->isBuilt()) continue;
            for (int t2 : trisV2) {
                if (t == t2) { owners.push_back(t); break; }
            }
        }

        std::vector<int> opposites;
        for (int ownerId : owners) {
            AbstractTriangle* owner = TriangleCollection::getTriangle(ownerId);
            if (owner->isBuilt()) continue;
            for (int i = 0; i < 3; ++i) {
                int v = owner->getVertex(i);
                if (v != edge[0] && v != edge[1]) opposites.push_back(v);
            }
            owner->markSuperseded();
        }

        if (owners.size() >= 2 && opposites.size() >= 2) {
            TriangleCollection::getTriangle(owners[0])->cloneWithVertices(opposites[0], opposites[1], edge[1]);
            TriangleCollection::getTriangle(owners[1])->cloneWithVertices(opposites[1], opposites[0], edge[0]);
        }
    }
}