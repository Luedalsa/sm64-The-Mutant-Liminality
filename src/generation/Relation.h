//
// Created by Luis Alvarez on 08/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_RELATION_H
#define SM64_THE_MUTANT_LIMINALITY_RELATION_H

#pragma once

#include "Dependent.h"

struct ProportionalRelation {
    Length stableSize = Length(0);
    ItemCount usesCount = ItemCount(0);
    //std::vector<StructuralEdge*> components;
/*
    std::vector<ProportionalRelation*>& getComponentsRelations() {
        static std::vector<ProportionalRelation*> allRelations;
        for (auto* edge : components) {
            if (edge->relation && std::find(allRelations.begin(), allRelations.end(), *edge->relation) == allRelations.end()) {
                allRelations.push_back(edge->relation);
            }
        }
        return allRelations;
    }*/
};

#endif // SM64_THE_MUTANT_LIMINALITY_RELATION_H
