// src/generation/AgentManager.cpp
#include "AgentManager.h"

#include "Agent.h"
#include "SemanticEdge.h"
#include "SemanticVertex.h"
#include "TriangleOverlapSolver.h"

#include <algorithm>
#include <queue>

std::priority_queue<Agent *> AgentManager::agentQueue;
std::priority_queue<SemanticVertex*> vertexQueue;
void AgentManager::start() {
    TriangleCollection::createTriangle(
        VertexCollection::createVertex({ 1, 0, 1 }), VertexCollection::createVertex({ 1, 0, -1 }),
        VertexCollection::createVertex({ -1, 0, -1 }), TriangleFaceType::CheckerboardFloor);

    std::vector<SemanticEdge> edges;
    auto compiled = SemanticCompiler::compile(edges);
    SemanticVertex* root = (new SemanticVertex());
    root->position = { 0, 0, 0 };
    root->relations = compiled[/* id del vértice raíz en el castillo base */ 0];
    vertexQueue.push(root);

    for (int i = 0; i < 256 && !vertexQueue.empty(); i++) {
        SemanticVertex* v = vertexQueue.top();
        vertexQueue.pop();

        //int mirror = rand() % 3; // TODO replace with positional random
        int mirror = 0;
        // getEdges filtra la arista de retorno (si parentEdge existe) y rebasa el yaw
        auto outgoing = SemanticCompiler::get(v->relations)->getEdges();
        int eself = -1;

        for (auto &e : *outgoing) {

        }

        //for (auto &e : *outgoing) {
            //if (e == eself) continue;
            auto e = (*outgoing)[rand() % outgoing->size()];
            auto semantic_vertex = (new SemanticVertex());
            semantic_vertex->parent = v;
            semantic_vertex->parentEdge = e;
            semantic_vertex->relations = edges[e].relations;
            vertexQueue.push(semantic_vertex);
            if (v->parentEdge == -1) { continue; }
            float yaw = (mirror == 1 ? 0 : (mirror == 0 ? edges[e].transform.yaw : -edges[e].transform.yaw)) + v->yaw; // TODO fix calc for abs theta
            semantic_vertex->yaw = yaw;
            semantic_vertex->position = v->position + Vector3(edges[e].transform.distance * std::cos(yaw), edges[e].transform.height, edges[e].transform.distance * std::sin(yaw));
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
        //}
    }

}