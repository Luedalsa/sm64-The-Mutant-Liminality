//
// Created by Luis Alvarez on 09/07/2026.
//
// TriangleOverlapSolver.cpp

#include "TriangleOverlapSolver.h"

#include "AbstractTriangle.h"
#include "DebugExport.h"
#include "TriangleCollection.h"

#include <array>

namespace {
using MyTri = std::array<Vector3, 3>;

std::vector<MyTri> clipFragmentSet(const std::vector<MyTri>& fragments, const Vector3& planePoint, const Vector3& planeNormal) {
    std::vector<MyTri> out;
    for (auto& f : fragments) {
        auto side = [&](const Vector3& p) { return (p - planePoint).dot(planeNormal); };

        std::vector<Vector3> input = { f[0], f[1], f[2] };
        std::vector<Vector3> output;

        for (size_t i = 0; i < input.size(); ++i) {
            const Vector3& current = input[i];
            const Vector3& next = input[(i + 1) % input.size()];
            float dCurrent = side(current);
            float dNext = side(next);

            bool currentInside = dCurrent <= 0.0f;
            bool nextInside = dNext <= 0.0f;

            if (currentInside) output.push_back(current);

            if (currentInside != nextInside) {
                float t = dCurrent / (dCurrent - dNext);
                output.push_back(current + (next - current) * t);
            }
        }

        std::vector<std::array<Vector3, 3>> result;
        if (output.size() < 3) return result;

        for (size_t i = 1; i + 1 < output.size(); ++i) {
            result.push_back({ output[0], output[i], output[i + 1] });
        }

        auto pieces = result;

        out.insert(out.end(), pieces.begin(), pieces.end());
    }
        return out;

    }


    Vector3 triNormal(const MyTri& t) {
        return (t[1] - t[0]).cross(t[2] - t[0]).normalized();
    }

    MyTri positionsOf(AbstractTriangle* t) {
        return {
            VertexCollection::getVertex(t->getVertex(0))->position,
            VertexCollection::getVertex(t->getVertex(1))->position,
            VertexCollection::getVertex(t->getVertex(2))->position
        };
    }
} // namespace

    void TriangleOverlapSolver::resolve(const std::vector<AgentTriangleRequest> &requested) {

    std::vector<MyTri> requestedPositions;
    requestedPositions.reserve(requested.size());
    for (auto& r : requested) requestedPositions.push_back({ r.v0, r.v1, r.v2 });

    std::vector<std::vector<int>> conflictsPerRequested(requested.size());

    // ── Pase 1: geometría global recortada contra la solicitada ──
    int globalCount = TriangleCollection::getTriangleCount();
    for (int g = 0; g < globalCount; ++g) {
        AbstractTriangle* triG = TriangleCollection::getTriangle(g);
        MyTri gPos = positionsOf(triG);

        std::vector<MyTri> fragments = { gPos };
        bool conflicted = false;

        for (size_t r = 0; r < requestedPositions.size(); ++r) {
            const MyTri& rPos = requestedPositions[r];

            Vector3 n = triNormal(rPos);
            auto next = clipFragmentSet(fragments, rPos[0], n);
            if (next.size() != fragments.size()) {
                conflicted = true;
                conflictsPerRequested[r].push_back(g);
            }
            fragments = std::move(next);
            if (fragments.empty()) break;
        }

        std::vector<Vector3> vertices;

        if (conflicted) {
            for (auto& frag : fragments) {
                vertices.push_back(frag[0]);
                int v1 = vertices.size();
                vertices.push_back(frag[1]);
                int v2 = vertices.size();
                vertices.push_back(frag[2]);
                int v3 = vertices.size();
            }
        }
    }

    for (const auto & i : requested) {
        auto a = VertexCollection::createVertex(i.v0);
        auto b = VertexCollection::createVertex(i.v1);
        auto c = VertexCollection::createVertex(i.v2);

        TriangleCollection::createTriangle<CheckerboardFloorTriangle>(a, b, c);
    }
}