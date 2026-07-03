//
// Created by Luis Alvarez on 16/06/2026.
//

#ifndef SM64_PORT_ABSTRACTVERTEX_H
#define SM64_PORT_ABSTRACTVERTEX_H
#include "types.h"

#include <vector>

class AbstractVertex {
    int collisionVertex = -1;
    public:
    Vec3f position;
    bool active = true;
    std::vector<int> connectingTriangles;
    AbstractVertex(float x, float y, float z) : position{x, y, z} {};

    int getCollisionVertex();;
};

#endif // SM64_PORT_ABSTRACTVERTEX_H
