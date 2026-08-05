//
// Created by Luis Alvarez on 11/07/2026.
//

#include "RoomsInterfaceSolver.h"

#include "VertexCollection.h"
#include "AbstractVertex.h"
#include "TriangleCollection.h"
#include "AbstractTriangle.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

uint64_t edgeKey(int va, int vb) {
    uint32_t lo = static_cast<uint32_t>(std::min(va, vb));
    uint32_t hi = static_cast<uint32_t>(std::max(va, vb));
    return (static_cast<uint64_t>(hi) << 32) | lo;
}

} // namespace

void RoomsInterfaceSolver::resolve() {
    int vertexCount = VertexCollection::getVertexCount();
    std::vector<bool> visited(vertexCount, false);
    int roomsFound = 0;

    for (int startV = 0; startV < vertexCount; ++startV) {
        if (visited[startV]) continue;

        std::vector<int> componentVertices;
        std::queue<int> frontier;
        frontier.push(startV);
        visited[startV] = true;

        while (!frontier.empty()) {
            int v = frontier.front();
            frontier.pop();
            componentVertices.push_back(v);

            for (int t : VertexCollection::getVertex(v)->connectingTriangles) {
                AbstractTriangle* tri = TriangleCollection::getTriangle(t);
                if (tri->isBuilt()) continue;

                for (int i = 0; i < 3; ++i) {
                    int nv = tri->getVertex(i);
                    if (nv >= 0 && nv < vertexCount && !visited[nv]) {
                        visited[nv] = true;
                        frontier.push(nv);
                    }
                }
            }
        }

        std::unordered_set<int> componentTriangles;
        std::unordered_map<uint64_t, int> edgeValence;

        for (int v : componentVertices) {
            for (int t : VertexCollection::getVertex(v)->connectingTriangles) {
                AbstractTriangle* tri = TriangleCollection::getTriangle(t);
                if (tri->isBuilt()) continue;
                if (!componentTriangles.insert(t).second) continue;

                for (int i = 0; i < 3; ++i) {
                    int va = tri->getVertex(i);
                    int vb = tri->getVertex((i + 1) % 3);
                    edgeValence[edgeKey(va, vb)]++;
                }
            }
        }

        if (componentTriangles.empty()) continue;

        bool isOpen = false;
        for (const auto& [key, valence] : edgeValence) {
            if (valence == 1) {
                isOpen = true;
                break;
            }
        }

        if (isOpen) {
            std::cerr << "Error: open geometry (component in vertex "
                      << startV << ", " << componentVertices.size() << " vertices, "
                      << componentTriangles.size() << " triangles)\n";
        } else {
            ++roomsFound;
            std::cout << "Found room! (component in vertex " << startV
                      << ", " << componentVertices.size() << " vertices, "
                      << componentTriangles.size() << " triangles)\n";
        }
    }

    std::cout << "RoomsInterfaceSolver found " << roomsFound << "room(s)\n";
}