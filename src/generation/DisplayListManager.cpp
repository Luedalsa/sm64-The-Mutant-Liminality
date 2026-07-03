//
// Created by Luis Alvarez on 03/07/2026.
//

#include "DisplayListManager.h"

std::vector<std::vector<Vtx>> DisplayListManager::displayVertices;
std::vector<std::vector<std::array<int, 3>>> DisplayListManager::displayTriangles;
std::vector<const u8*> DisplayListManager::displayTextures;