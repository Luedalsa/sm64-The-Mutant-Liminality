//
// Created by Luis Alvarez on 16/06/2026.
//

#ifndef SM64_PORT_ABSTRACTVERTEX_H
#define SM64_PORT_ABSTRACTVERTEX_H
#include "Primitives.h"
#include "types.h"

#include <vector>

class AbstractVertex {
    int collisionVertex = -1;
    public:
    Vector3 position;
    bool active = true;
    std::vector<int> connectingTriangles;
    AbstractVertex(Vector3 v) : position(v) {};

    int getCollisionVertex();;
};

#endif // SM64_PORT_ABSTRACTVERTEX_H
