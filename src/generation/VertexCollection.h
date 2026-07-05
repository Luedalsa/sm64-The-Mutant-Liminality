//
// Created by Luis Alvarez on 20/06/2026.
//

#ifndef SM64_PORT_VERTEXCOLLECTION_H
#define SM64_PORT_VERTEXCOLLECTION_H
#include "Primitives.h"

extern "C" {
    #include "types.h"
}

#include <deque>
#include <vector>
#include <functional>

class AbstractVertex;

class VertexCollection {
    static std::deque<AbstractVertex> vertices;

public:
    static void forEachVertexIntersectingBox(
        const Vec3f boxStart,
        const Vec3f boxEnd,
        std::function<void(const AbstractVertex&)> callback
    );

    // ¡Solo ponemos las firmas!
    static int createVertex(Vector3 v);
    static int getVertexCount();
    static AbstractVertex* getVertex(int v);
};

#endif // SM64_PORT_VERTEXCOLLECTION_H