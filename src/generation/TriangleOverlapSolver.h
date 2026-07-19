//
// Created by Luis Alvarez on 09/07/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_TRIANGLEOVERLAPSOLVER_H
#define SM64_THE_MUTANT_LIMINALITY_TRIANGLEOVERLAPSOLVER_H

#include "VertexCollection.h"
#include <vector>
#include <functional>

struct AgentTriangleRequest {
    Vector3 v0, v1, v2;
    std::function<int(int, int, int)> factory; // ej: [](int a,int b,int c){ return TriangleCollection::createTriangle<LavaTriangle>(a,b,c); }
};

class TriangleOverlapSolver {
public:
    static void resolve(const std::vector<AgentTriangleRequest>& requested);
};

#endif // SM64_THE_MUTANT_LIMINALITY_TRIANGLEOVERLAPSOLVER_H
