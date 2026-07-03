//
// Created by Luis Alvarez on 17/06/2026.
//

#ifndef SM64_PORT_TRIANGLECOLLECTION_H
#define SM64_PORT_TRIANGLECOLLECTION_H
#include "AbstractTriangle.h"

class AbstractTriangle;

extern "C" {
    #include "types.h"
}

#include <deque>
#include <functional>

class TriangleCollection {
    static std::deque<AbstractTriangle> triangles;

public:
    static void forEachTriangleIntersectingBox(
        const Vec3f boxStart,
        const Vec3f boxEnd,
        std::function<void(const AbstractTriangle&)> callback
    );

    static int createTriangle(int v1, int v2, int v3);
/*
    static AbstractTriangle* createTriangle(AbstractTriangle* tri) {
        auto newtri = new AbstractTriangle(tri->getVertex(0), tri->getVertex(1), tri->getVertex(2));
        triangles.push_back(newtri);
        return newtri;
    }*/
    static AbstractTriangle* getTriangle(int t) {
        return &triangles[t];
    };
};

#endif // SM64_PORT_TRIANGLECOLLECTION_H
