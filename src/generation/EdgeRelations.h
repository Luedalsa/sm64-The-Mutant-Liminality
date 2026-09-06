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
    void addEdge(int edge, int weight) { edges.push_back(edge); weights.push_back(weight); }

    std::deque<int>* getEdges();
};

#endif // SM64_THE_MUTANT_LIMINALITY_EDGERELATIONS_H