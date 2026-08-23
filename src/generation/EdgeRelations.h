//
// Created by Luis Alvarez on 21/08/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_EDGERELATIONS_H
#define SM64_THE_MUTANT_LIMINALITY_EDGERELATIONS_H

#include <vector>
#include <deque>

struct SemanticEdge;

class SemanticRelations {
    std::deque<int> edges;
public:
    std::vector<int> weights;

    SemanticRelations() = default;
    void addEdge(int edge) { edges.push_back(edge); }

    std::deque<int>* getEdges();
};

#endif // SM64_THE_MUTANT_LIMINALITY_EDGERELATIONS_H