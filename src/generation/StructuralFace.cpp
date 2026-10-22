//
// Created by Luis Alvarez on 19/09/2026.
//

#include "StructuralFace.h"

#include "AgentManager.h"
#include "DisplayListManager.h"
#include "StructuralEdge.h"

#include <array>
#include <cmath>
#include <vector>

namespace {


} // namespace

extern "C" {
    #include "surface_terrains.h"
    #include "textures.h"
    #include "engine/surface_load.h"
}

StructuralFace::StructuralFace() {
}
StructuralFace::~StructuralFace() {
    throw NonRecoverableContradictionException(
        "StructuralFace destructor called. This should not happen, as StructuralFaces are managed by "
        "the system and should not be deleted manually.");
}
void StructuralFace::collapseTerrain() {
    sSurfacePool[0].type = SURFACE_DEFAULT;
    sSurfacePool[0].vertex1[0] = AgentManager::pivotEdge->getHalfEdge(0).structuralEdge.position[0].value;
    sSurfacePool[0].vertex1[1] = AgentManager::pivotEdge->getHalfEdge(0).structuralEdge.position[1].value;
    sSurfacePool[0].vertex1[2] = AgentManager::pivotEdge->getHalfEdge(0).structuralEdge.position[2].value;
    sSurfacePool[0].vertex2[0] = AgentManager::pivotEdge->getHalfEdge(0).walk().structuralEdge.position[0].value;
    sSurfacePool[0].vertex2[1] = AgentManager::pivotEdge->getHalfEdge(0).walk().structuralEdge.position[1].value;
    sSurfacePool[0].vertex2[2] = AgentManager::pivotEdge->getHalfEdge(0).walk().structuralEdge.position[2].value;
    sSurfacePool[0].vertex3[0] = AgentManager::pivotEdge->getHalfEdge(0).walk().walk().structuralEdge.position[0].value;
    sSurfacePool[0].vertex3[1] = AgentManager::pivotEdge->getHalfEdge(0).walk().walk().structuralEdge.position[1].value;
    sSurfacePool[0].vertex3[2] = AgentManager::pivotEdge->getHalfEdge(0).walk().walk().structuralEdge.position[2].value;

    sSurfacePool[1].type = SURFACE_DEFAULT;
    sSurfacePool[1].vertex1[0] = AgentManager::pivotEdge->getHalfEdge(0).walk().walk().structuralEdge.position[0].value;
    sSurfacePool[1].vertex1[1] = AgentManager::pivotEdge->getHalfEdge(0).walk().walk().structuralEdge.position[1].value;
    sSurfacePool[1].vertex1[2] = AgentManager::pivotEdge->getHalfEdge(0).walk().walk().structuralEdge.position[2].value;
    sSurfacePool[1].vertex2[0] = AgentManager::pivotEdge->getHalfEdge(0).walk().walk().walk().structuralEdge.position[0].value;
    sSurfacePool[1].vertex2[1] = AgentManager::pivotEdge->getHalfEdge(0).walk().walk().walk().structuralEdge.position[1].value;
    sSurfacePool[1].vertex2[2] = AgentManager::pivotEdge->getHalfEdge(0).walk().walk().walk().structuralEdge.position[2].value;
    sSurfacePool[1].vertex3[0] = AgentManager::pivotEdge->getHalfEdge(0).structuralEdge.position[0].value;
    sSurfacePool[1].vertex3[1] = AgentManager::pivotEdge->getHalfEdge(0).structuralEdge.position[1].value;
    sSurfacePool[1].vertex3[2] = AgentManager::pivotEdge->getHalfEdge(0).structuralEdge.position[2].value;
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
        {0, 2, 3}
    };

    DisplayListManager::addDisplayListSegment(vertices, triangles, inside_09001000);
}
