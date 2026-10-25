//
// Created by Luis Alvarez on 24/10/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_SPACEANCHOR_H
#define SM64_THE_MUTANT_LIMINALITY_SPACEANCHOR_H

#include "StructuralEdge.h"
#include "BaseEdge.h"

struct SpaceAnchor {
    static HalfEdge *pivotHalfEdge;
    static RelativeTransform pivotTransform;
};

#endif // SM64_THE_MUTANT_LIMINALITY_SPACEANCHOR_H
