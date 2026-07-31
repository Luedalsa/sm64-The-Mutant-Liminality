//
// Created by Luis Alvarez on 17/06/2026.
//

#ifndef SM64_PORT_TRIANGLECOLLECTION_H
#define SM64_PORT_TRIANGLECOLLECTION_H
#include "AbstractTriangle.h"
#include <memory>

class AbstractTriangle;

extern "C" {
    #include "types.h"
}

#include <deque>
#include <functional>

class TriangleCollection {
    static std::deque<std::unique_ptr<AbstractTriangle>> triangles;

public:
    static int getTriangleCount() { return static_cast<int>(triangles.size()); }

    static void forEachTriangleIntersectingBox(
        const Vec3f boxStart,
        const Vec3f boxEnd,
        std::function<void(const AbstractTriangle&)> callback
    );

    static int createTriangle(int v1, int v2, int v3,
                               TriangleFaceType frontType,
                               TriangleFaceType backType = TriangleFaceType::None);

    static AbstractTriangle* getTriangle(int t) { return triangles[t].get(); }

    static void markSuperseded(int triangleId) { getTriangle(triangleId)->markSuperseded(); }
};

#endif // SM64_PORT_TRIANGLECOLLECTION_H
