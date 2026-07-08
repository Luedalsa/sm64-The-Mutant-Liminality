//
// Created by Luis Alvarez on 16/06/2026.
//

#include "LevelScriptManager.h"
#include "VertexCollection.h"
#include "AbstractTriangle.h"
#include "AbstractVertex.h"

#include <map>

AbstractTriangle::AbstractTriangle(int v1,
                                   int v2,
                                   int v3)
{
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;

    const auto & a = VertexCollection::getVertex(v1)->position;
    const auto & b = VertexCollection::getVertex(v2)->position;
    const auto & c = VertexCollection::getVertex(v3)->position;

    const Vector3 u = b - a;
    const Vector3 v = c - a;

    normal = u.cross(v).normalized();
}

void AbstractTriangle::build() {
    if (built) return;
    built = true;

    std::vector<int> trianglesToBuild;
    trianglesToBuild.push_back(selfIndex);

    std::queue<int> frontier;

    for (int i = 0; i < 3; ++i) {
        for (auto t : VertexCollection::getVertex(vertices[i])->connectingTriangles) {
            frontier.push(t);
        }
    }

    while (!frontier.empty()) {
        int t = frontier.front();
        frontier.pop();

        AbstractTriangle* neighbor = TriangleCollection::getTriangle(t);

        if (neighbor == this) continue;

        if (typeid(*neighbor) == typeid(*this)) {
            if (neighbor->built) continue;
            neighbor->built = true;
            trianglesToBuild.push_back(t);

            for (int i = 0; i < 3; ++i) {
                for (auto t2 : VertexCollection::getVertex(neighbor->vertices[i])->connectingTriangles) {
                    frontier.push(t2);
                }
            }
        } else {
            LevelScriptManager::addTriangleToBuildQueue(t);
        }
    }

    buildGeometry(trianglesToBuild);
}

void CheckerboardFloorTriangle::buildGeometry(std::vector<int>& neighbours) {
    std::map<int, int> localVertexSegment;
    std::vector<std::array<int, 3>> localTriangleSegment;
    std::vector<DisplayVertex> displayVertices;
    int currentVertex = 0;
    for (auto n : neighbours) {
        auto tri = TriangleCollection::getTriangle(n);
        AbstractVertex* vertexa = VertexCollection::getVertex(tri->getVertex(0));
        AbstractVertex* vertexb = VertexCollection::getVertex(tri->getVertex(1));
        AbstractVertex* vertexc = VertexCollection::getVertex(tri->getVertex(2));
        int vv1 = vertexa->getCollisionVertex();
        int vv2 = vertexb->getCollisionVertex();
        int vv3 = vertexc->getCollisionVertex();
        CollisionManager::addCollisionTriangle(vv1, vv2, vv3, SURFACE_DEFAULT);

        for (int i = 0; i < 3; ++i) {
            int v = tri->getVertex(i);
            if (localVertexSegment.find(v) == localVertexSegment.end()) {
                localVertexSegment[v] = currentVertex++;
                displayVertices.push_back({VertexCollection::getVertex(v)->position, 0, VertexCollection::getVertex(v)->position.x, VertexCollection::getVertex(v)->position.z, {0, 127, 0}});
            }
        }
        localTriangleSegment.push_back({localVertexSegment[tri->getVertex(0)], localVertexSegment[tri->getVertex(1)], localVertexSegment[tri->getVertex(2)]});
    }

    DisplayListManager::addDisplayListSegment(displayVertices, localTriangleSegment, inside_09004000);
}


void LavaTriangle::buildGeometry(std::vector<int>& neighbours) {
    std::map<int, int> localVertexSegment;
    std::vector<std::array<int, 3>> localTriangleSegment;
    std::vector<DisplayVertex> displayVertices;
    int currentVertex = 0;
    for (auto n : neighbours) {
        auto tri = TriangleCollection::getTriangle(n);
        AbstractVertex* vertexa = VertexCollection::getVertex(tri->getVertex(0));
        AbstractVertex* vertexb = VertexCollection::getVertex(tri->getVertex(1));
        AbstractVertex* vertexc = VertexCollection::getVertex(tri->getVertex(2));
        int vv1 = vertexa->getCollisionVertex();
        int vv2 = vertexb->getCollisionVertex();
        int vv3 = vertexc->getCollisionVertex();
        CollisionManager::addCollisionTriangle(vv1, vv2, vv3, SURFACE_BURNING);

        for (int i = 0; i < 3; ++i) {
            int v = tri->getVertex(i);
            if (localVertexSegment.find(v) == localVertexSegment.end()) {
                localVertexSegment[v] = currentVertex++;
                displayVertices.push_back({VertexCollection::getVertex(v)->position, 0, VertexCollection::getVertex(v)->position.x, VertexCollection::getVertex(v)->position.z, {0, 127, 0}});
            }
        }
        localTriangleSegment.push_back({localVertexSegment[tri->getVertex(0)], localVertexSegment[tri->getVertex(1)], localVertexSegment[tri->getVertex(2)]});
    }

    DisplayListManager::addDisplayListSegment(displayVertices, localTriangleSegment, fire_09009000);
}