//
// Created by Luis Alvarez on 04/07/2026.
//

#include "AgentManager.h"

#include "Agent.h"
#include "TriangleOverlapSolver.h"

#include <queue>

std::priority_queue<Agent *> AgentManager::agentQueue;
void AgentManager::start() {
    TriangleCollection::createTriangle(
        VertexCollection::createVertex({ 1, 0, 1 }), VertexCollection::createVertex({ 1, 0, -1 }),
        VertexCollection::createVertex({ -1, 0, -1 }), TriangleFaceType::CheckerboardFloor);
    DebugAgent ok(SurfaceTransform{ 0, 0.5, 0.5, { 0, 0.1, -0.7071 } });
    agentQueue.push(&ok);
    while (!agentQueue.empty()) {
        auto agent = agentQueue.top();
        agentQueue.pop();
        agent->grow();
    }
}