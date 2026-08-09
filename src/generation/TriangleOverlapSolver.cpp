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

#include <algorithm>
#include <queue>
#include <stack>

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

bool pointBarycentric(const Vector3& p, int triId, float& u, float& v, float& w) {
    AbstractTriangle* tri = TriangleCollection::getTriangle(triId);
    const Vector3& a = VertexCollection::getVertex(tri->getVertex(0))->position;
    const Vector3& b = VertexCollection::getVertex(tri->getVertex(1))->position;
    const Vector3& c = VertexCollection::getVertex(tri->getVertex(2))->position;

    Vector3 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = v0.dot(v0), d01 = v0.dot(v1), d11 = v1.dot(v1);
    float d20 = v2.dot(v0), d21 = v2.dot(v1);
    float denom = d00 * d11 - d01 * d01;
    if (std::abs(denom) < 1e-9f) return false;

    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;
    return true;
}

// PARCHE (temporal, pre-rediseño): agrupa todos los crossings Strong interiores
// que caen sobre targetTriId antes de partir, en vez de solo el primero.
// Rastrea en qué fragmento cae cada punto subsecuente para no perderlos.
// NO reemplaza el polygon-clip pendiente: si un punto no cae limpio en ningún
// fragmento actual (numéricamente al borde), se descarta -- mejor perder un
// crossing marginal que generar un sliver corrupto.
void resolveStrongMultiSplit(int targetTriId, const std::vector<EdgeCrossing>& interiorCrossings,
                              std::queue<int>& pending) {
    std::vector<int> fragments = { targetTriId };

    for (const auto& c : interiorCrossings) {
        int fragmentHit = -1;
        for (int f : fragments) {
            float u, v, w;
            if (!pointBarycentric(c.point, f, u, v, w)) continue;
            if (u >= -TriangleEdgeSplitSolver::kBaryMargin &&
                v >= -TriangleEdgeSplitSolver::kBaryMargin &&
                w >= -TriangleEdgeSplitSolver::kBaryMargin) {
                fragmentHit = f;
                break;
            }
        }
        if (fragmentHit == -1) continue;

        int newVertex = TemporalVertexRegistry::getOrCreate(c.point, /*asStrong=*/true, 1.0f);
        auto children = TriangleEdgeSplitSolver::splitTriangleAtInteriorPoint(fragmentHit, newVertex);
        splitEdgeAcrossOwners(c.va, c.vb, newVertex);

        fragments.erase(std::remove(fragments.begin(), fragments.end(), fragmentHit), fragments.end());
        for (int child : children) fragments.push_back(child);
    }

    for (int f : fragments) pending.push(f);
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
    std::stack<std::array<int, 2>> sleepIsForTheWeak;
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
        /*
        for (auto& c : crossingsDir1)
            if (c.strength == EdgeStrength::Weak)
                sleepIsForTheWeak.push({c.va, c.vb});
        for (auto& c : crossingsDir2)
            if (c.strength == EdgeStrength::Weak)
                sleepIsForTheWeak.push({c.va, c.vb});*/

        bool resolvedStrong = false;
        std::vector<EdgeCrossing> strongInteriorDir1;

        for (auto& c : crossingsDir1) {
            //if (c.strength != EdgeStrength::Strong) continue;
            if (c.hitEdgeIndex != -1) {
                resolveEdgeCrossingSplit(triId, c.hitEdgeIndex, c.point, pending);
                resolvedStrong = true;
                break;
            }
            strongInteriorDir1.push_back(c);
        }

        if (!resolvedStrong && !strongInteriorDir1.empty()) {
            resolveStrongMultiSplit(triId, strongInteriorDir1, pending);
            resolvedStrong = true;
        }

        if (!resolvedStrong) {
            std::vector<EdgeCrossing> strongInteriorDir2;
            for (auto& c : crossingsDir2) {
                //if (c.strength != EdgeStrength::Strong) continue;
                if (c.hitEdgeIndex != -1) {
                    resolveEdgeCrossingSplit(otherId, c.hitEdgeIndex, c.point, pending);
                    resolvedStrong = true;
                    break;
                }
                strongInteriorDir2.push_back(c);
            }
            if (!resolvedStrong && !strongInteriorDir2.empty()) {
                resolveStrongMultiSplit(otherId, strongInteriorDir2, pending);
                resolvedStrong = true;
            }
        }

        if (resolvedStrong) break;
    }
}
while (!sleepIsForTheWeak.empty()) {
        auto edge = sleepIsForTheWeak.top();
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
        }

        if (owners.size() == 2 && opposites.size() == 2) {
            int fwd = -1, bwd = -1;
            for (size_t i = 0; i < owners.size(); ++i) {
                AbstractTriangle* owner = TriangleCollection::getTriangle(owners[i]);
                int idxA = -1, idxB = -1;
                for (int k = 0; k < 3; ++k) {
                    if (owner->getVertex(k) == edge[0]) idxA = k;
                    else if (owner->getVertex(k) == edge[1]) idxB = k;
                }
                if (idxA == -1 || idxB == -1) continue;
                if ((idxA + 1) % 3 == idxB) fwd = static_cast<int>(i);
                else bwd = static_cast<int>(i);
            }

            if (fwd != -1 && bwd != -1) {
                int oppFwd = opposites[fwd];
                int oppBwd = opposites[bwd];

                TriangleCollection::getTriangle(owners[0])->markSuperseded();
                TriangleCollection::getTriangle(owners[1])->markSuperseded();

                TriangleCollection::getTriangle(owners[fwd])->cloneWithVertices(edge[0], oppBwd, oppFwd);
                TriangleCollection::getTriangle(owners[bwd])->cloneWithVertices(oppFwd, oppBwd, edge[1]);
            }
        } else {
            std::cerr << "Warning: Edge (" << edge[0] << ", " << edge[1] << ") has "
                 << owners.size() << " owners and " << opposites.size() << " opposites. This edge shouldn't have been classified as weak.\n";
        }
    }
}