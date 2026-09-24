//
// Created by Luis Alvarez on 15/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_BASEEDGE_H
#define SM64_THE_MUTANT_LIMINALITY_BASEEDGE_H
#include "TriangleTypes.h"
#include "UVModes.h"

#include <utility>

struct RelativeTransform
{
    float distance;
    float yaw;
    float height;
};

struct BaseEdge {
    virtual ~BaseEdge() = default;
    RelativeTransform transform{0.0f, 0, 0.0f};

    int32_t id = -1;
    int32_t relation = -1;
    float positionHint = 0.0f;
    float orientation = 0.0f;
    float size = 0.0f;
    bool orientationLocked = false;
    bool merged = false;
    int32_t mergedInto = -1;
};

#endif // SM64_THE_MUTANT_LIMINALITY_BASEEDGE_H
