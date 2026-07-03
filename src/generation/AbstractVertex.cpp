//
// Created by Luis Alvarez on 16/06/2026.
//

#include "LevelScriptManager.h"
#include "AbstractVertex.h"

int AbstractVertex::getCollisionVertex() {
    if (collisionVertex == -1) {
        collisionVertex = CollisionManager::addCollisionVertex(position[0], position[1], position[2]);
    }
    return collisionVertex;
}