//
// Created by Luis Alvarez on 17/06/2026.
//

#include "VertexCollection.h"
#include "TriangleCollection.h"

#include <iostream>

std::deque<std::unique_ptr<AbstractTriangle>> TriangleCollection::triangles;


void TriangleCollection::forEachTriangleIntersectingBox(
    const Vec3f boxStart,
    const Vec3f boxEnd,
    std::function<void(const AbstractTriangle&)> callback
) {/*
    for (AbstractTriangle tri : triangles) {
        if (true) {
            callback(tri);
        }
    }*/
}
