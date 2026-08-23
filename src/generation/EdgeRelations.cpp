//
// Created by Luis Alvarez on 21/08/2026.
//

#include "EdgeRelations.h"
#include "SemanticEdge.h"
#include <deque>

std::deque<SemanticEdge> SemanticRelations::getEdges(SemanticRelations* semanticKey) const {
    SemanticEdge* e = nullptr;
    auto earr = *(new std::deque<SemanticEdge>());
    for (auto edge : edges) {
        if (edge.relations == semanticKey) e = &edge; else earr.push_back(edge);
    }
    if (e == nullptr) {
        std::cerr << "No semantic edge found! This is an invalid relation" << std::endl;
        return earr;
    }
    for (auto edge : earr) {
        edge.transform.yaw -= e->transform.yaw + 3.141592f;
    }
    return earr;
}