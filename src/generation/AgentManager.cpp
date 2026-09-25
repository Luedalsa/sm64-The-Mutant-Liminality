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

    StructuralEdge edge = std::move(*new StructuralEdge());
    StructuralSurface surface = std::move(*new StructuralSurface());

    pivotEdge = &edge;
    pivotSurface = &surface;

    surface.collapseDisplayList();
}
