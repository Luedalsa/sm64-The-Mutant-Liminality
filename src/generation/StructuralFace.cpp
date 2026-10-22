//
// Created by Luis Alvarez on 19/09/2026.
//

#include "StructuralFace.h"

#include "AgentManager.h"
#include "DisplayListManager.h"
#include "StructuralEdge.h"
#include "textures.h"

#include <array>
#include <cmath>
#include <vector>

namespace {


} // namespace

StructuralFace::StructuralFace() {
}
StructuralFace::~StructuralFace() {
    throw NonRecoverableContradictionException(
        "StructuralFace destructor called. This should not happen, as StructuralFaces are managed by "
        "the system and should not be deleted manually.");
}
void StructuralFace::collapseTerrain() {
}
void StructuralFace::collapseDisplayList() {

    std::vector<DisplayVertex> vertices;
    HalfEdge* edge = &AgentManager::pivotEdge->getHalfEdge(0);
    do {
        vertices.emplace_back((DisplayVertex){{edge->structuralEdge.position[0].value, edge->structuralEdge.position[1].value, edge->structuralEdge.position[2].value}, 0, 0, 0, Vector3::zero()});
        edge = &edge->walk();
    } while (edge != &AgentManager::pivotEdge->getHalfEdge(0));

    std::vector<std::array<int, 3>> triangles = {
        {0, 1, 2},
        //{0, 2, 3}
    };

    DisplayListManager::addDisplayListSegment(vertices, triangles, inside_09001000);
}
