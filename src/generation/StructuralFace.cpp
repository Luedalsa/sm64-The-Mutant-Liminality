//
// Created by Luis Alvarez on 19/09/2026.
//

#include "StructuralFace.h"
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
void StructuralFace::collapseDisplayList() {/*
    if (edges.size() < 3) {
        throw NonRecoverableContradictionException("Cannot collapse display list with less than 3 edges.");
    }

    std::vector<DisplayVertex> vertices;
    for (StructuralEdge * edge : edges) {
        vertices.emplace_back((DisplayVertex){edge->getPosition(), 0, 0, 0, Vector3::zero()});
    }

    std::vector<std::array<int, 3>> triangles = {
        {0, 1, 2},
        {0, 2, 3}
    };

    DisplayListManager::addDisplayListSegment(vertices, triangles, inside_09001000);*/
}
