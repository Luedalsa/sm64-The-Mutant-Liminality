// src/generation/AgentManager.cpp
#include "AgentManager.h"

#include "Agent.h"
#include "SemanticEdge.h"
#include "SemanticVertex.h"
#include "TriangleOverlapSolver.h"

#include <queue>

std::priority_queue<Agent *> AgentManager::agentQueue;
std::priority_queue<SemanticVertex> vertexQueue;
void AgentManager::start() {
    TriangleCollection::createTriangle(
        VertexCollection::createVertex({ 1, 0, 1 }), VertexCollection::createVertex({ 1, 0, -1 }),
        VertexCollection::createVertex({ -1, 0, -1 }), TriangleFaceType::CheckerboardFloor);

    auto compiled = SemanticCompiler::compile();
    SemanticVertex root;
    root.position = { 0, 0, 0 };
    root.relations = compiled[/* id del vértice raíz en el castillo base */ 0];
    vertexQueue.push(root);

    for (int i = 0; i < 256 && !vertexQueue.empty(); i++) {
        SemanticVertex v = vertexQueue.top();
        vertexQueue.pop();

        //int mirror = rand() % 3; // TODO replace with positional random
        int mirror = 0;
        // getEdges filtra la arista de retorno (si parentEdge existe) y rebasa el yaw
        auto outgoing = v.relations->getEdges(v.parentEdge ? v.parentEdge->relations : nullptr);

        for (auto &e : outgoing) {
            auto semantic_vertex = *(new SemanticVertex());
            semantic_vertex.parent = &v;
            semantic_vertex.parentEdge = &e;
            semantic_vertex.relations = e.relations;
            semantic_vertex.yaw = v.yaw + (mirror == 1 ? 0 : (mirror == 0 ? e.transform.yaw : -e.transform.yaw));
            semantic_vertex.position = v.position + Vector3(e.transform.distance * std::cos(e.transform.yaw + v.yaw), e.transform.height, e.transform.distance * std::sin(e.transform.yaw + v.yaw));
            vertexQueue.push(semantic_vertex);
            if (v.parentEdge == nullptr) { continue; }
            for (auto triangle : e.triangles) {
                for (auto i : v.parentEdge->triangles) {
                    if (triangle == i) {
                        std::cout << std::endl;
                        TriangleCollection::createTriangle(
                            VertexCollection::createVertex(v              .position),
                            VertexCollection::createVertex(semantic_vertex.position),
                            VertexCollection::createVertex(v.parent      ->position),
                            triangle);
                    }
                }
            }
        }
    }

}