// src/generation/AgentManager.cpp
#include "AgentManager.h"

#include "Constraintsolver.h"
#include "SemanticCompiler.h"
#include "SemanticEdge.h"
#include "SemanticVertex.h"
#include "StructuralEdge.h"
#include "StructuralFace.h"

#include <cmath>
#include <iostream>
#include <queue>

bool invalidGeometry = true;
StructuralFace* pivotSurface = nullptr;
StructuralEdge* pivotEdge = nullptr;
RelativeTransform pivotTransform = RelativeTransform();

// Las estructuras de datos están diseñadas para evitar vulnerabilidades en las estructuras de datos durante la generación de geometría y solución de restricciones. evitando infernales sesiones de debug.
// Algunas claves de mi metodología:
// Todo es referencia hasta que se demuestre la necesidad de un puntero nulo
// Todo es privado hasta que se demuestre la necesidad de una interfaz
// Todo es constante hasta que se demuestre la necesidad de una variable
// Todo es singleton hasta que se demuestre la necesidad de instancias múltiples
void AgentManager::start() {
    invalidGeometry = true;
    TriangleCollection::createTriangle(
        VertexCollection::createVertex({ 1, 0, 1 }),
        VertexCollection::createVertex({ 1, 0, -1 }),
        VertexCollection::createVertex({ -1, 0, -1 }),
        TriangleFaceType::CheckerboardFloor);

    ProportionalRelation *relation1 = (new ProportionalRelation{1000.0f});

    StructuralEdge *edges = (StructuralEdge*)malloc(sizeof(StructuralEdge) * 12);
/*
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
    }*/

    static StructuralFace surfaces[6] = {};
/*
    // S0: tapa inferior (invertida respecto a los lados)
    edges[0].halfEdges.first.changeNextEdge(edges[3]);
    edges[3].halfEdges.first.changeNextEdge(edges[2]);
    edges[2].halfEdges.first.changeNextEdge(edges[1]);
    edges[1].halfEdges.first.changeNextEdge(edges[0]);

    // S1: tapa superior
    edges[8].halfEdges.first.changeNextEdge(edges[9]);
    edges[9].halfEdges.first.changeNextEdge(edges[10]);
    edges[10].halfEdges.first.changeNextEdge(edges[11]);
    edges[11].halfEdges.first.changeNextEdge(edges[8]);

    // S2
    edges[0].halfEdges.second.changeNextEdge(edges[5]);
    edges[5].halfEdges.first.changeNextEdge(edges[8]);
    edges[8].halfEdges.second.changeNextEdge(edges[4]);
    edges[4].halfEdges.first.changeNextEdge(edges[0]);

    // S3
    edges[1].halfEdges.second.changeNextEdge(edges[6]);
    edges[6].halfEdges.first.changeNextEdge(edges[9]);
    edges[9].halfEdges.second.changeNextEdge(edges[5]);
    edges[5].halfEdges.second.changeNextEdge(edges[1]);

    // S4
    edges[2].halfEdges.second.changeNextEdge(edges[7]);
    edges[7].halfEdges.first.changeNextEdge(edges[10]);
    edges[10].halfEdges.second.changeNextEdge(edges[6]);
    edges[6].halfEdges.second.changeNextEdge(edges[2]);

    // S5
    edges[3].halfEdges.second.changeNextEdge(edges[4]);
    edges[4].halfEdges.second.changeNextEdge(edges[11]);
    edges[11].halfEdges.second.changeNextEdge(edges[7]);
    edges[7].halfEdges.second.changeNextEdge(edges[3]);

    // Dirección inversa: cada arista sabe cuáles dos superficies la comparten.
    edges[0].halfEdges.first.changeSurface(surfaces[0]);
    edges[0].halfEdges.second.changeSurface(surfaces[2]);
    edges[1].halfEdges.first.changeSurface(surfaces[0]);
    edges[1].halfEdges.second.changeSurface(surfaces[3]);
    edges[2].halfEdges.first.changeSurface(surfaces[0]);
    edges[2].halfEdges.second.changeSurface(surfaces[4]);
    edges[3].halfEdges.first.changeSurface(surfaces[0]);
    edges[3].halfEdges.second.changeSurface(surfaces[5]);
    edges[8].halfEdges.first.changeSurface(surfaces[1]);
    edges[8].halfEdges.second.changeSurface(surfaces[2]);
    edges[9].halfEdges.first.changeSurface(surfaces[1]);
    edges[9].halfEdges.second.changeSurface(surfaces[3]);
    edges[10].halfEdges.first.changeSurface(surfaces[1]);
    edges[10].halfEdges.second.changeSurface(surfaces[4]);
    edges[11].halfEdges.first.changeSurface(surfaces[1]);
    edges[11].halfEdges.second.changeSurface(surfaces[5]);
    edges[4].halfEdges.first.changeSurface(surfaces[2]);
    edges[4].halfEdges.second.changeSurface(surfaces[5]);
    edges[5].halfEdges.first.changeSurface(surfaces[2]);
    edges[5].halfEdges.second.changeSurface(surfaces[3]);
    edges[6].halfEdges.first.changeSurface(surfaces[3]);
    edges[6].halfEdges.second.changeSurface(surfaces[4]);
    edges[7].halfEdges.first.changeSurface(surfaces[4]);
    edges[7].halfEdges.second.changeSurface(surfaces[5]);
*/

    auto* e = static_cast<StructuralEdge *>(malloc(sizeof(StructuralEdge)));
    std::vector<StructuralEdge*> e2;
    StructuralFace* newSurfaceA;

    BiologicalAbomination::abiogenesis(e);
    BiologicalAbomination::cellWallGrow(e->getHalfEdge(0), e->getHalfEdge(1).walk().walk(), e2, newSurfaceA);

    pivotEdge = e;
    pivotSurface = newSurfaceA;
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
    newSurfaceA->collapseDisplayList();
}