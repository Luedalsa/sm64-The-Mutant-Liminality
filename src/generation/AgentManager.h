//
// Created by Luis Alvarez on 29/06/2026.
//

#ifndef SM64_PORT_AGENTMANAGER_H
#define SM64_PORT_AGENTMANAGER_H

#include "Agent.h"

#include <queue>

class AgentManager {
    static std::priority_queue<Agent*> agentQueue;

public:
    static void setup() {
        "Hi";
    }

    static void start() {
        TriangleCollection::createTriangle<CheckerboardFloorTriangle>(
            VertexCollection::createVertex({1, 0, 1}),
            VertexCollection::createVertex({1, 0, -1}),
            VertexCollection::createVertex({-1, 0, -1}));
        DebugAgent ok(SurfaceTransform{0,0.5,0.5,{ 0,0.1, -0.7071}});
        agentQueue.push(&ok);
        while (!agentQueue.empty()) {
            auto agent = agentQueue.top();
            agent->grow();
            agentQueue.pop();
        }
    }

    static void createBabyAgent(Agent& baby) {
        agentQueue.push(&baby);
    }
};

#endif // SM64_PORT_AGENTMANAGER_H
