//
// Created by Luis Alvarez on 29/06/2026.
//

#ifndef SM64_PORT_AGENTMANAGER_H
#define SM64_PORT_AGENTMANAGER_H

#include "StructuralFace.h"
#include "TriangleTypes.h"

#include <queue>

class Agent;

class AgentManager {

public:
    static void setup() {
        registerBuiltinTriangleTypes();
    }

    static StructuralFace *pivotSurface;
    static StructuralEdge *pivotEdge;
    static RelativeTransform pivotTransform;
    static void start();

};

#endif // SM64_PORT_AGENTMANAGER_H
