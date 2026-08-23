// src/generation/AgentManager.cpp
#include "AgentManager.h"

#include "Agent.h"
#include "SemanticEdge.h"
#include "SemanticVertex.h"
#include "TriangleOverlapSolver.h"

#include <algorithm>
#include <queue>

std::priority_queue<Agent *> AgentManager::agentQueue;
std::deque<SemanticVertex*> vertexQueue;
void AgentManager::start() {
    TriangleCollection::createTriangle(
        VertexCollection::createVertex({ 1, 0, 1 }), VertexCollection::createVertex({ 1, 0, -1 }),
        VertexCollection::createVertex({ -1, 0, -1 }), TriangleFaceType::CheckerboardFloor);

    std::vector<SemanticEdge> edges;
    auto compiled = SemanticCompiler::compile(edges);
    SemanticVertex* root = (new SemanticVertex());
    root->position = { 0, 0, 0 };
    root->relations = compiled[/* id del vértice raíz en el castillo base */ 0];
    vertexQueue.push_back(root);

    for (int i = 0; i < 256 && !vertexQueue.empty(); i++) {
        SemanticVertex* v = vertexQueue.front();
        vertexQueue.pop_front();

        //int mirror = rand() % 3; // TODO replace with positional random
        int mirror = 0;
        // getEdges filtra la arista de retorno (si parentEdge existe) y rebasa el yaw
        auto outgoing = SemanticCompiler::get(v->relations)->getEdges();
        int eself = -1;
        float yawoff = 0.0f;
        for (auto &e : *outgoing) {
            if (v->parentEdge != -1) {
                eself = e;
                yawoff = edges[e].transform.yaw;
                break;
            }
        }

        for (auto &e : *outgoing) {
            if (e == eself) continue;
            auto semantic_vertex = (new SemanticVertex());
            semantic_vertex->parent = v;
            semantic_vertex->parentEdge = e;
            semantic_vertex->relations = edges[e].relations;
            semantic_vertex->yaw = v->yaw + yawoff + (mirror == 1 ? 0 : (mirror == 0 ? edges[e].transform.yaw : -edges[e].transform.yaw));
            semantic_vertex->position = v->position + Vector3(edges[e].transform.distance * std::cos(edges[e].transform.yaw + v->yaw), edges[e].transform.height, edges[e].transform.distance * std::sin(edges[e].transform.yaw + v->yaw));
            vertexQueue.push_back(semantic_vertex);
            if (v->parentEdge == -1) { continue; }
            for (auto triangle : edges[e].triangles) {
                for (auto i : edges[v->parentEdge].triangles) {
                    if (triangle == i) {
                        std::cout << std::endl;
                        TriangleCollection::createTriangle(
                            VertexCollection::createVertex(v              ->position),
                            VertexCollection::createVertex(semantic_vertex->position),
                            VertexCollection::createVertex(v->parent      ->position),
                            triangle);
                    }
                }
            }
        }
    }

}