//
// Created by Luis Alvarez on 17/06/2026.
//

#include "VertexCollection.h"
#include "TriangleCollection.h"

std::deque<AbstractTriangle> TriangleCollection::triangles;


void TriangleCollection::forEachTriangleIntersectingBox(
    const Vec3f boxStart,
    const Vec3f boxEnd,
    std::function<void(const AbstractTriangle&)> callback
) {
    for (AbstractTriangle tri : triangles) {
        if (true) {
            callback(tri);
        }
    }
}


int TriangleCollection::createTriangle(int v1, int v2, int v3) {
    auto tri = AbstractTriangle(v1, v2, v3);
    triangles.push_back(tri);
    int id = triangles.size() - 1;
    VertexCollection::getVertex(v1)->connectingTriangles.push_back(id);
    VertexCollection::getVertex(v2)->connectingTriangles.push_back(id);
    VertexCollection::getVertex(v3)->connectingTriangles.push_back(id);
    return id;
}