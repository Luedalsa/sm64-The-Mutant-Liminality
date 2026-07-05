//
// Created by Luis Alvarez on 03/07/2026.
//

#include <vector>
#include <unordered_map>

#include "CollisionManager.h"

#include "Primitives.h"

std::vector<Vector3> CollisionManager::collisionVertices;
std::unordered_map<int, std::vector<int>> CollisionManager::collisionTriangles;
