//
// Created by Luis Alvarez on 20/06/2026.
//

#include "VertexCollection.h"
#include "AbstractVertex.h"

std::deque<AbstractVertex> VertexCollection::vertices;

void VertexCollection::forEachVertexIntersectingBox(
    const Vec3f boxStart,
    const Vec3f boxEnd,
    std::function<void(const AbstractVertex&)> callback
) {
    for (AbstractVertex vtx : VertexCollection::vertices) {
        if (true) {
            callback(vtx);
        }
    }
}


int VertexCollection::createVertex(Vector3 v) {
    // Aquí el compilador ya sabe exactamente cuánto mide y cómo se construye
    auto vtx = AbstractVertex(v);
    vertices.push_back(vtx);
    return vertices.size()-1;
}

int VertexCollection::getVertexCount() {
    return vertices.size();
}

AbstractVertex* VertexCollection::getVertex(int v) {
    return &vertices[v];
}
