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

namespace {
std::priority_queue<SemanticVertex *> vertexQueue;
}

StructuralSurface* pivotSurface = nullptr;
StructuralEdge* pivotEdge = nullptr;
RelativeTransform pivotTransform = RelativeTransform();

void AgentManager::start() {
    TriangleCollection::createTriangle(
        VertexCollection::createVertex({ 1, 0, 1 }),
        VertexCollection::createVertex({ 1, 0, -1 }),
        VertexCollection::createVertex({ -1, 0, -1 }),
        TriangleFaceType::CheckerboardFloor);

    Relation *relation1 = (new Relation{1000.0f});

    StructuralEdge edge11 = *(new StructuralEdge());
    edge11.relation = relation1;
    StructuralEdge edge12 = *(new StructuralEdge());
    edge12.relation = relation1;
    StructuralEdge edge13 = *(new StructuralEdge());
    edge13.relation = relation1;
    StructuralEdge edge14 = *(new StructuralEdge());
    edge14.relation = relation1;

    StructuralEdge edge21 = *(new StructuralEdge());
    edge21.relation = relation1;
    StructuralEdge edge22 = *(new StructuralEdge());
    edge22.relation = relation1;
    StructuralEdge edge23 = *(new StructuralEdge());
    edge23.relation = relation1;
    StructuralEdge edge24 = *(new StructuralEdge());
    edge24.relation = relation1;

    StructuralEdge edge31 = *(new StructuralEdge());
    edge31.relation = relation1;
    StructuralEdge edge32 = *(new StructuralEdge());
    edge32.relation = relation1;
    StructuralEdge edge33 = *(new StructuralEdge());
    edge33.relation = relation1;
    StructuralEdge edge34 = *(new StructuralEdge());
    edge34.relation = relation1;

    StructuralSurface surface1 = std::move(*new StructuralSurface());
    surface1.edges = {&edge11, &edge12, &edge13, &edge14};

    StructuralSurface surface2 = std::move(*new StructuralSurface()); // techo
    surface2.edges = {&edge31, &edge32, &edge33, &edge34};

    StructuralSurface surface3 = std::move(*new StructuralSurface()); // pared 1
    surface3.edges = {&edge11, &edge22, &edge31, &edge21};

    StructuralSurface surface4 = std::move(*new StructuralSurface()); // pared 2
    surface4.edges = {&edge12, &edge23, &edge32, &edge22};

    StructuralSurface surface5 = std::move(*new StructuralSurface()); // pared 3
    surface5.edges = {&edge13, &edge24, &edge33, &edge23};

    StructuralSurface surface6 = std::move(*new StructuralSurface()); // pared 4
    surface6.edges = {&edge14, &edge21, &edge34, &edge24};

    // Dirección inversa: cada arista sabe cuáles dos superficies la comparten.
    edge11.surfaces = {&surface1, &surface3};
    edge12.surfaces = {&surface1, &surface4};
    edge13.surfaces = {&surface1, &surface5};
    edge14.surfaces = {&surface1, &surface6};

    edge31.surfaces = {&surface2, &surface3};
    edge32.surfaces = {&surface2, &surface4};
    edge33.surfaces = {&surface2, &surface5};
    edge34.surfaces = {&surface2, &surface6};

    edge21.surfaces = {&surface3, &surface6};
    edge22.surfaces = {&surface3, &surface4};
    edge23.surfaces = {&surface4, &surface5};
    edge24.surfaces = {&surface5, &surface6};

    pivotEdge = &edge11;
    pivotSurface = &surface1;
    pivotTransform = RelativeTransform{1000.0f, 3.141592f/2.0f, 0.0f};

    //Structural edge position collapse

    surface1.collapseDisplayList();
}
