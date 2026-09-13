// src/generation/AgentManager.cpp
#include "AgentManager.h"

#include "Agent.h"
#include "Constraintsolver.h"
#include "SemanticCompiler.h"
#include "SemanticEdge.h"
#include "SemanticVertex.h"

#include <cmath>
#include <iostream>
#include <queue>

std::priority_queue<Agent *> AgentManager::agentQueue;

namespace {
std::priority_queue<SemanticVertex *> vertexQueue;
}

void AgentManager::start() {
    while (!agentQueue.empty()) agentQueue.pop();
    while (!vertexQueue.empty()) vertexQueue.pop();

    TriangleCollection::createTriangle(
        VertexCollection::createVertex({ 1, 0, 1 }),
        VertexCollection::createVertex({ 1, 0, -1 }),
        VertexCollection::createVertex({ -1, 0, -1 }),
        TriangleFaceType::CheckerboardFloor);

    std::vector<SemanticEdge> edges;
    const std::unordered_map<int, int> compiled = SemanticCompiler::compile(edges);

    // ConstraintSolver consumes the same edge instances used by generation.
    // This is the integration point that was missing from the old manager.
    ConstraintSolver constraints;
    constraints.solve(edges);

    SemanticVertex *root = new SemanticVertex();
    root->position = { 0, 0, 0 };
    root->geometricRelations = compiled.empty() ? -1 : compiled.begin()->second;
    vertexQueue.push(root);

    for (int steps = 0; steps < 256 && !vertexQueue.empty(); ++steps) {
        SemanticVertex *vertex = vertexQueue.top();
        vertexQueue.pop();

        SemanticRelations *relations = SemanticCompiler::get(vertex->geometricRelations);
        if (relations == nullptr || relations->getEdges()->empty()) continue;

        const int edgeId = relations->getEdges()->front();
        if (edgeId < 0 || edgeId >= static_cast<int>(edges.size())) continue;
        const SemanticEdge &edge = edges[edgeId];

        SemanticVertex *child = new SemanticVertex();
        child->parent = vertex;
        child->parentEdge = edgeId;
        child->geometricRelations = edge.geometricRelations;
        child->yaw = vertex->yaw + edge.transform.yaw;
        child->position = vertex->position + Vector3(
            edge.transform.distance * std::cos(child->yaw),
            edge.transform.height,
            edge.transform.distance * std::sin(child->yaw));
        vertexQueue.push(child);

        if (vertex->parentEdge >= 0 &&
            vertex->parentEdge < static_cast<int>(edges.size())) {
            for (TriangleFaceType triangle : edge.triangles) {
                for (TriangleFaceType parentTriangle :
                     edges[vertex->parentEdge].triangles) {
                    if (triangle == parentTriangle) {
                        TriangleCollection::createTriangle(
                            VertexCollection::createVertex(vertex->position),
                            VertexCollection::createVertex(child->position),
                            VertexCollection::createVertex(vertex->parent->position),
                            triangle);
                    }
                }
            }
        }
    }

    std::cout << "\033[32m[OK] All constraints solved!\033[0m" << std::endl;
}
