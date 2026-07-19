//
// Created by Luis Alvarez on 04/07/2026.
//

#include "AgentManager.h"

#include "Agent.h"
#include "TriangleOverlapSolver.h"

#include <queue>

std::priority_queue<Agent *> AgentManager::agentQueue;
void AgentManager::start() {
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(
        VertexCollection::createVertex({ 1, 0, 1 }), VertexCollection::createVertex({ 1, 0, -1 }),
        VertexCollection::createVertex({ -1, 0, -1 }));
    DebugAgent ok(SurfaceTransform{ 0, 0.5, 0.5, { 0, 0.1, -0.7071 } });
    agentQueue.push(&ok);
    while (!agentQueue.empty()) {
        auto agent = agentQueue.top();
        agentQueue.pop();
        agent->grow();
    }
    std::vector<AgentTriangleRequest> cubeFaces;
    Vector3 p000(300,800,-2000), p100(3000,800,-2000), p110(3000,800,-500), p010(300,800,-500);
    Vector3 p001(300, 2000,-2000), p101(3000, 2000,-2000), p111(3000, 2000,-500), p011(300, 2000,-500);

    auto floorFactory = [](int a, int b, int c) { return TriangleCollection::createTriangle<CheckerboardFloorTriangle>(a, b, c); };

    // Abajo (y = -1000), normal hacia +y
    cubeFaces.push_back({p000, p110, p100, floorFactory});
    cubeFaces.push_back({p000, p010, p110, floorFactory});

    // Arriba (y = +1000), normal hacia -y
    cubeFaces.push_back({p001, p101, p111, floorFactory});
    cubeFaces.push_back({p001, p111, p011, floorFactory});

    // Frente (z = -1000), normal hacia +z
    cubeFaces.push_back({p000, p100, p101, floorFactory});
    cubeFaces.push_back({p000, p101, p001, floorFactory});

    // Atrás (z = +1000), normal hacia -z
    cubeFaces.push_back({p010, p111, p110, floorFactory});
    cubeFaces.push_back({p010, p011, p111, floorFactory});

    // Izquierda (x = 1000), normal hacia +x
    cubeFaces.push_back({p000, p011, p010, floorFactory});
    cubeFaces.push_back({p000, p001, p011, floorFactory});

    // Derecha (x = 3000), normal hacia -x
    cubeFaces.push_back({p100, p110, p111, floorFactory});
    cubeFaces.push_back({p100, p111, p101, floorFactory});

    TriangleOverlapSolver::resolve(cubeFaces);
}