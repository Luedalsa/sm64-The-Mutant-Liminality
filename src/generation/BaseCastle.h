//
// Created by Luis Alvarez on 15/08/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_BASECASTLE_H
#define SM64_THE_MUTANT_LIMINALITY_BASECASTLE_H
#include "AbstractTriangle.h"
#include "SemanticEdge.h"

struct SemanticEdge;

struct BaseCastle {
    static constexpr size_t SEMANTIC_EDGE_COUNT = 1;
    static const SemanticEdge semanticEdges[SEMANTIC_EDGE_COUNT];
};

#endif // SM64_THE_MUTANT_LIMINALITY_BASECASTLE_H
