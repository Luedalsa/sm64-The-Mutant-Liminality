//
// Created by Luis Alvarez on 21/08/2026.
//

#include "EdgeRelations.h"
#include "SemanticEdge.h"
#include <deque>

std::deque<int>* SemanticRelations::getEdges() {
    return &edges;
}