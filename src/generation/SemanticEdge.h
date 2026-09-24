//
// Created by Luis Alvarez on 20/08/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_SEMANTICEDGE_H
#define SM64_THE_MUTANT_LIMINALITY_SEMANTICEDGE_H
#include "AbstractTriangle.h"
#include "BaseCastle.h"
#include "BaseEdge.h"
#include "EdgeRelations.h"

#include <vector>

struct SemanticEdge : BaseEdge {
    std::pair<TriangleFaceType, TriangleFaceType> idealFaceTypes = {TriangleFaceType::None, TriangleFaceType::None};
    int geometricRelations = -1;
};

#endif // SM64_THE_MUTANT_LIMINALITY_SEMANTICEDGE_H