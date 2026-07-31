//
// Created by Luis Alvarez on 16/06/2026.
//

#include "LevelScriptManager.h"
#include "VertexCollection.h"
#include "AbstractTriangle.h"
#include "AbstractVertex.h"

AbstractTriangle::AbstractTriangle(int v1, int v2, int v3,
                                    TriangleFaceType frontType_,
                                    TriangleFaceType backType_)
    : frontType(frontType_), backType(backType_)
{
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;
    active = true;

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

        if (neighbor->frontType == frontType && neighbor->backType == backType) {
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

    TriangleTypeRegistry::build(frontType, trianglesToBuild);
}

int AbstractTriangle::cloneWithVertices(int v1, int v2, int v3) const {
    return TriangleCollection::createTriangle(v1, v2, v3, frontType, backType);
}