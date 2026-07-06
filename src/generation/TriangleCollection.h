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
    static void forEachTriangleIntersectingBox(
        const Vec3f boxStart,
        const Vec3f boxEnd,
        std::function<void(const AbstractTriangle&)> callback
    );
    template <class T = AbstractTriangle, class... Args> static int createTriangle(Args &&...args);

    static AbstractTriangle* getTriangle(int t) {
        return triangles[t].get();
    };
};

#include "TriangleCollection.tpp"

#endif // SM64_PORT_TRIANGLECOLLECTION_H
