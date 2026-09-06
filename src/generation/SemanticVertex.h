//
// Created by Luis Alvarez on 20/08/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_SEMANTICVERTEX_H
#define SM64_THE_MUTANT_LIMINALITY_SEMANTICVERTEX_H
#include "SemanticCompiler.h"
#include "SemanticEdge.h"

#include <vector>

struct SemanticVertex {
    Vector3 position;
    float yaw = 0.0f;
    int relations = -1;
    SemanticVertex* parent = nullptr;
    int parentEdge = -1;

    bool operator< (const SemanticVertex& other) const { // TODO use degeneration to decide priority, most degenerated ones go first
        return std::sqrt(float(position.x * position.x + position.z * position.z)) < std::sqrt(float(other.position.x * other.position.x + other.position.z * other.position.z));
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_SEMANTICVERTEX_H
