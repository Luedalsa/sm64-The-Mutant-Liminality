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
    auto newLevel = buildLevelScript();
    return newLevel;
}