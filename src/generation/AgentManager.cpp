// src/generation/AgentManager.cpp
#include "AgentManager.h"

#include "Constraintsolver.h"
#include "SemanticCompiler.h"
#include "SemanticEdge.h"
#include "SemanticVertex.h"
#include "StructuralEdge.h"
#include "StructuralSurface.h"

#include <cmath>
#include <iostream>
#include <queue>

bool invalidGeometry = true;
StructuralSurface* pivotSurface = nullptr;
StructuralEdge* pivotEdge = nullptr;
RelativeTransform pivotTransform = RelativeTransform();

// Las estructuras de datos están diseñadas para evitar vulnerabilidades en las estructuras de datos durante la generación de geometría y solución de restricciones. evitando infernales sesiones de debug.
void AgentManager::start() {
    invalidGeometry = true;
    TriangleCollection::createTriangle(
        VertexCollection::createVertex({ 1, 0, 1 }),
        VertexCollection::createVertex({ 1, 0, -1 }),
        VertexCollection::createVertex({ -1, 0, -1 }),
        TriangleFaceType::CheckerboardFloor);

    ProportionalRelation *relation1 = (new ProportionalRelation{1000.0f});

    struct PrimitiveStructuralEdge : StructuralEdge {
    };

    StructuralEdge *edges = (StructuralEdge*)malloc(sizeof(PrimitiveStructuralEdge) * 12);

    edges[0].relation = relation1;
    edges[1].relation = relation1;
    edges[2].relation = relation1;
    edges[3].relation = relation1;

    edges[4].relation = relation1;
    edges[5].relation = relation1;
    edges[6].relation = relation1;
    edges[7].relation = relation1;

    edges[8].relation = relation1;
    edges[9].relation = relation1;
    edges[10].relation = relation1;
    edges[11].relation = relation1;

    for (int i = 0; i < 12; ++i) {
        edges[i].stableSeparation = 3.14159265358979323846f / 2.0f;
        edges[i].idealSeparation = 3.14159265358979323846f / 2.0f;
    }


    struct PrimitiveStructuralSurface : StructuralSurface {
    };

    static PrimitiveStructuralSurface surfaces[6] = {};
/*
    surfaces[0].edges = {&edges[0], &edges[1], &edges[2], &edges[3]};
    surfaces[1].edges = {&edges[8], &edges[9], &edges[10], &edges[11]};
    surfaces[2].edges = {&edges[0], &edges[5], &edges[8], &edges[4]};
    surfaces[3].edges = {&edges[1], &edges[6], &edges[9], &edges[5]};
    surfaces[4].edges = {&edges[2], &edges[7], &edges[10], &edges[6]};
    surfaces[5].edges = {&edges[3], &edges[4], &edges[11], &edges[7]};

    edges[0].nextEdges = {&edges[1], &edges[5]};
        edges[1].nextEdges = {&edges[2], &edges[6]};
        edges[2].nextEdges = {&edges[3], &edges[7]};
        edges[3].nextEdges = {&edges[0], &edges[4]};
        edges[4].nextEdges = {&edges[0], &edges[11]};
        edges[5].nextEdges = {&edges[1], &edges[8]};*/

    // Dirección inversa: cada arista sabe cuáles dos superficies la comparten.
    edges[0].changeSurfaceA(surfaces[0]);
    edges[0].changeSurfaceB(surfaces[2]);
    edges[1].changeSurfaceA(surfaces[0]);
    edges[1].changeSurfaceB(surfaces[3]);
    edges[2].changeSurfaceA(surfaces[0]);
    edges[2].changeSurfaceB(surfaces[4]);
    edges[3].changeSurfaceA(surfaces[0]);
    edges[3].changeSurfaceB(surfaces[5]);
    edges[8].changeSurfaceA(surfaces[1]);
    edges[8].changeSurfaceB(surfaces[2]);
    edges[9].changeSurfaceA(surfaces[1]);
    edges[9].changeSurfaceB(surfaces[3]);
    edges[10].changeSurfaceA(surfaces[1]);
    edges[10].changeSurfaceB(surfaces[4]);
    edges[11].changeSurfaceA(surfaces[1]);
    edges[11].changeSurfaceB(surfaces[5]);
    edges[4].changeSurfaceA(surfaces[2]);
    edges[4].changeSurfaceB(surfaces[5]);
    edges[5].changeSurfaceA(surfaces[2]);
    edges[5].changeSurfaceB(surfaces[3]);
    edges[6].changeSurfaceA(surfaces[3]);
    edges[6].changeSurfaceB(surfaces[4]);
    edges[7].changeSurfaceA(surfaces[4]);
    edges[7].changeSurfaceB(surfaces[5]);


    pivotEdge = reinterpret_cast<StructuralEdge*>(&edges[0]);
    pivotSurface = reinterpret_cast<StructuralSurface*>(&surfaces[0]);
    pivotTransform = RelativeTransform{1000.0f, 3.141592f/2.0f, 0.0f};
/*
    pivotSurface->normal.set(Vector3(0, 1, 0));
    pivotSurface->normal.setPriority(ConstraintSolver::currentFrame);
    pivotEdge->position.set(Vector3(1000, 0, 0));
    pivotEdge->position.setPriority(ConstraintSolver::currentFrame);
        nextEdges.first->position = Vector3(pos.x + transform.distance * cos(transform.yaw),
                                            pos.y + transform.height,
                                            pos.z + transform.distance * sin(transform.yaw));
        nextEdges.first->position.priority = ConstraintSolver::currentFrame;
    StructuralEdge* next = (pivotEdge->getSurfaces().first == pivotSurface ? pivotEdge->getNextEdges().first : pivotEdge->getNextEdges().second);
    next->setPosition(Vector3(1000, 0, 1000));*/
    reinterpret_cast<StructuralSurface*>(&surfaces[0])->collapseDisplayList();
}
