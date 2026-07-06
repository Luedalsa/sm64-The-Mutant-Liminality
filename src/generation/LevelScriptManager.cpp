//
// Created by Luis Alvarez on 30/06/2026.
//

#include "LevelScriptManager.h"

#include "AbstractVertex.h"
#include "TriangleCollection.h"

std::queue<int> LevelScriptManager::trianglesQueue;

/**
 *
 * @return A pointer to a LevelScript that represents the built level, including collision data, display
 * lists, and other necessary information for the game engine to render and interact with the level.
 */
LevelScript * LevelScriptManager::buildLevel() {

    addTriangleToBuildQueue(0);

    while (trianglesQueue.size() > 0) {
        auto t = trianglesQueue.front();
        trianglesQueue.pop();
        auto triangle = TriangleCollection::getTriangle(t);
        if (triangle->isBuilt() == false) {
            triangle->build();
        }
    }

    auto newLevel = buildLevelScript();
    return newLevel;
}