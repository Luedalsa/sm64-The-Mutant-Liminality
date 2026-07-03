//
// Created by Luis Alvarez on 03/07/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_COLLISIONMANAGER_H
#define SM64_THE_MUTANT_LIMINALITY_COLLISIONMANAGER_H
#include "surface_terrains.h"

#include <unordered_map>
#include <vector>

class CollisionManager {
    static std::vector<int> collisionVertices;
    static std::unordered_map<int, std::vector<int>> collisionTriangles;
public:

    static int addCollisionVertex(int x, int y, int z) {
        int s = collisionVertices.size()/3;
        collisionVertices.push_back(x);
        collisionVertices.push_back(y);
        collisionVertices.push_back(z);
        return s;
    }

    static void addCollisionTriangle(int v1, int v2, int v3, int surface = SURFACE_DEFAULT) {
        collisionTriangles[surface].push_back(v1);
        collisionTriangles[surface].push_back(v2);
        collisionTriangles[surface].push_back(v3);
    }

    static const std::vector<int>& getCollisionVertices() {
        return collisionVertices;
    }

    static const std::unordered_map<int, std::vector<int>>& getCollisionTriangles() {
        return collisionTriangles;
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_COLLISIONMANAGER_H
