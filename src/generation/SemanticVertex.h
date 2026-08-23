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
    float yaw = 0;
    SemanticRelations* relations = nullptr;
    SemanticVertex* parent = nullptr;
    SemanticEdge* parentEdge = nullptr;

    bool operator< (const SemanticVertex& other) const { // TODO use degeneration to decide priority, most degenerated ones go first
        return position.length() > other.position.length();
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_SEMANTICVERTEX_H
