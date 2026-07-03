//
// Created by Luis Alvarez on 30/06/2026.
//

#include "LevelScriptManager.h"

#include "AbstractVertex.h"
#include "TriangleCollection.h"

std::vector<void*> LevelScriptManager::poolPointers;
std::vector<int> LevelScriptManager::collisionVertices;
std::unordered_map<int, std::vector<int>> LevelScriptManager::collisionTriangles;
std::vector<std::vector<Vtx>> LevelScriptManager::displayVertices;
std::vector<std::vector<std::array<int, 3>>> LevelScriptManager::displayTriangles;
std::vector<const u8*> LevelScriptManager::displayTextures;
std::vector<std::array<int, 4>> LevelScriptManager::doors;
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
        if (triangle->built == false) {
            triangle->build();
        }
    }

    auto newLevel = buildLevelScript();
    return newLevel;
}