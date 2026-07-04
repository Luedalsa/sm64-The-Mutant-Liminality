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
        DebugAgent ok;
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
