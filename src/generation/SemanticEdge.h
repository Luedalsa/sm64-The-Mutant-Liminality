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
    int geometricRelations = -1;
    std::vector<TriangleFaceType> triangles = {};
    RelativeTransform transform{0.0f, 0, 0.0f};

    // Runtime state used by ConstraintSolver.  These fields intentionally
    // live on the semantic edge so agents and the solver observe the same
    // object instead of maintaining two incompatible edge models.
    int32_t id = -1;
    // Compatibility alias for the constraint model.  geometricRelations is
    // the compiler's name; both are normalized by ConstraintSolver.
    int32_t relation = -1;
    float positionHint = 0.0f;
    float orientation = 0.0f;
    float size = 0.0f;
    bool orientationLocked = false;
    bool merged = false;
    int32_t mergedInto = -1;
};

#endif // SM64_THE_MUTANT_LIMINALITY_SEMANTICEDGE_H