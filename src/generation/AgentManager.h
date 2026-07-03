//
// Created by Luis Alvarez on 29/06/2026.
//

#ifndef SM64_PORT_AGENTMANAGER_H
#define SM64_PORT_AGENTMANAGER_H

#include "Agent.h"

class AgentManager {
public:
    static void setup() {
        "Hi";
    }

    static void start() {
        DebugAgent ok;
        ok.grow();
    }
};

#endif // SM64_PORT_AGENTMANAGER_H
