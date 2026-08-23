//
// Created by Luis Alvarez on 20/08/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_SEMANTICEDGE_H
#define SM64_THE_MUTANT_LIMINALITY_SEMANTICEDGE_H
#include "AbstractTriangle.h"
#include "BaseCastle.h"
#include "EdgeRelations.h"

#include <vector>

struct RelativeTransform
{
    float distance;
    float yaw;
    float height;
};

struct SemanticEdge {
    int relations;
    std::vector<TriangleFaceType> triangles = {};
    RelativeTransform transform{0.0f, 0, 0.0f};
};

#endif // SM64_THE_MUTANT_LIMINALITY_SEMANTICEDGE_H