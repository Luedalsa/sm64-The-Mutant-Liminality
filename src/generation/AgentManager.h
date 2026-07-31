//
// Created by Luis Alvarez on 29/06/2026.
//

#ifndef SM64_PORT_AGENTMANAGER_H
#define SM64_PORT_AGENTMANAGER_H

#include "Agent.h"

#include <queue>

class Agent;

class AgentManager {
    static std::priority_queue<Agent*> agentQueue;

public:
    static void setup() {
        registerBuiltinTriangleTypes();
    }

    static void start();

    static void createBabyAgent(Agent& baby) {
        agentQueue.push(&baby);
    }
};

#endif // SM64_PORT_AGENTMANAGER_H
