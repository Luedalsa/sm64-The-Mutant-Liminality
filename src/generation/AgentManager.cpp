// src/generation/AgentManager.cpp
#include "AgentManager.h"

#include "Agent.h"
#include "TriangleOverlapSolver.h"

#include <queue>

std::priority_queue<Agent *> AgentManager::agentQueue;

void AgentManager::start() {
    TriangleCollection::createTriangle(
        VertexCollection::createVertex({ 1, 0, 1 }), VertexCollection::createVertex({ 1, 0, -1 }),
        VertexCollection::createVertex({ -1, 0, -1 }), TriangleFaceType::CheckerboardFloor);

    Agent ok(Vector3(0, 0, 0), Gene{});
    Agent ok2(Vector3(500, 500, 500), Gene{});
    agentQueue.push(&ok);
    agentQueue.push(&ok2);
    while (!agentQueue.empty()) {
        auto agent = agentQueue.top();
        agentQueue.pop();
        agent->grow();
    }
}