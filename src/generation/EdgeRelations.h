//
// Created by Luis Alvarez on 21/08/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_EDGERELATIONS_H
#define SM64_THE_MUTANT_LIMINALITY_EDGERELATIONS_H

#include <vector>
#include <deque>

struct SemanticEdge;

class SemanticRelations {
    std::deque<SemanticEdge> edges;
public:
    std::vector<int> weights;

    SemanticRelations() = default;
    void addEdge(const SemanticEdge& edge) { edges.push_back(edge); }

    std::deque<SemanticEdge>* getEdges(int semanticKey) const;
};

#endif // SM64_THE_MUTANT_LIMINALITY_EDGERELATIONS_H