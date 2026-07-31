//
// Created by Luis Alvarez on 09/07/2026.
// Reescrito 19/07/2026 para integrar TriangleEdgeSplitSolver.
//
// TriangleOverlapSolver.cpp

#include "TriangleOverlapSolver.h"

#include "AbstractTriangle.h"
#include "DebugExport.h"
#include "TriangleCollection.h"
#include "TriangleEdgeSplitSolver.h"

#include <queue>

namespace {

// Broad-phase ingenuo: bounding box de los 3 vértices con un pequeño margen.
// TODO: reemplazar por VertexCollection/TriangleCollection::forEachTriangleIntersectingBox
// cuando dejen de ser stubs (ver code review anterior) — esto es O(n^2) por diseño,
// aceptable para el minimal path pero no para niveles grandes.
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

// Split-en-2 propagado a TODOS los dueños non-manifold del edge (va,vb),
// sin tocar la cola pending — el llamador decide qué encolar.
// Extraído de resolveStrongEdgeSplit para reusarlo también en el caso
// edge-on-edge (resolveEdgeCrossingSplit).
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

// NUEVO: caso "split en 2" — el punto de cruce cae sobre un edge propio de
// triId. triId es simplemente uno de los dueños de ese edge; se propaga
// igual que cualquier owner non-manifold (hasta 4 triángulos compartiendo
// el mismo edge, ej. piso/techo de rooms apiladas).
void resolveEdgeCrossingSplit(int triId, int edgeIndex, const Vector3& point, std::queue<int>& pending) {
    AbstractTriangle* tri = TriangleCollection::getTriangle(triId);
    int ea = tri->getVertex(edgeIndex);
    int eb = tri->getVertex((edgeIndex + 1) % 3);

    int newVertex = TemporalVertexRegistry::getOrCreate(point, /*asStrong=*/true, 1.0f);

    // Guard: si la cuantización de TemporalVertexRegistry "snapeó" el punto
    // a un vértice ya existente del propio edge (caso degenerado, punto muy
    // pegado a un extremo), no hay nada que splitear.
    if (newVertex == ea || newVertex == eb) return;

    int beforeCount = TriangleCollection::getTriangleCount();
    splitEdgeAcrossOwners(ea, eb, newVertex);
    int afterCount = TriangleCollection::getTriangleCount();

    for (int newId = beforeCount; newId < afterCount; ++newId) {
        pending.push(newId);
    }
}

} // namespace

void TriangleOverlapSolver::resolve(const std::vector<AgentTriangleRequest>& requested) {

    std::vector<int> committed;
    committed.reserve(requested.size());
    for (const auto& r : requested) {
        int a = VertexCollection::createVertex(r.v0);
        int b = VertexCollection::createVertex(r.v1);
        int c = VertexCollection::createVertex(r.v2);

        int id = r.factory
            ? r.factory(a, b, c)
            : TriangleCollection::createTriangle(a, b, c, TriangleFaceType::CheckerboardFloor);

        committed.push_back(id);
    }

    // ── Paso 2: cola de conflictos pendientes ──
    // Empezamos con lo recién comprometido. Cada vez que una resolución
    // (split o flip) genera triángulos nuevos, los volvemos a encolar,
    // porque ellos también pueden entrar en conflicto con el resto.
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

        bool triSuperseded = false;

        for (int otherId = 0; otherId < total; ++otherId) {
            if (otherId == triId) continue;

            AbstractTriangle* other = TriangleCollection::getTriangle(otherId);
            if (other->isBuilt()) continue;
            if (!aabbOverlap(boxA, triangleBounds(otherId))) continue;

            bool resolvedThisPair = false;

            auto crossingsDir1 = detectCrossings(otherId, triId);
            auto crossingsDir2 = detectCrossings(triId, otherId);

            for (auto& c : crossingsDir1) {
                if (c.hitEdgeIndex == -1 && c.strength == EdgeStrength::Weak) sleepIsForTheWeak.push({c.va, c.vb});
            }
            for (auto& c : crossingsDir2) {
                if (c.hitEdgeIndex == -1 && c.strength == EdgeStrength::Weak) sleepIsForTheWeak.push({c.va, c.vb});
            }

            // Primer caso "duro" (Strong interior O edge-on-edge) gana, en ese orden
            // de aparición dentro de cada lista.
            for (auto& c : crossingsDir1) {
                if (c.hitEdgeIndex != -1) {
                    resolveEdgeCrossingSplit(triId, c.hitEdgeIndex, c.point, pending);
                    resolvedThisPair = true;
                    break;
                }
                if (c.strength == EdgeStrength::Strong) {
                    resolveStrongEdgeSplit(c.va, c.vb, c.point, triId, pending);
                    resolvedThisPair = true;
                    break;
                }
            }
            if (!resolvedThisPair) {
                for (auto& c : crossingsDir2) {
                    if (c.hitEdgeIndex != -1) {
                        resolveEdgeCrossingSplit(otherId, c.hitEdgeIndex, c.point, pending);
                        resolvedThisPair = true;
                        break;
                    }
                    if (c.strength == EdgeStrength::Strong) {
                        resolveStrongEdgeSplit(c.va, c.vb, c.point, otherId, pending);
                        resolvedThisPair = true;
                        break;
                    }
                }
            }

            if (resolvedThisPair) { triSuperseded = true; break; }
        }
    }

    // ── Fix del bug de tamaño mutable: usar while, no for con size() ──
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