// src/generation/Agent.h
#ifndef SM64_PORT_AGENT_H
#define SM64_PORT_AGENT_H
#include "VertexCollection.h"
#include "TriangleCollection.h"
#include "AbstractVertex.h"
#include "AgentManager.h"
#include "Primitives.h"
#include "Gene.h"

extern "C" {
#include "engine/math_util.h"
}

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include <cmath>

// ─────────────────────────────────────────────────────────────────
// Agent
//
// Nace en una posición del mundo con un Gen heredado (stub por ahora).
// Al crecer: invoca un Prism (que crea sus triángulos directamente en
// TriangleCollection) y manda los IDs resultantes a TriangleOverlapSolver
// para que se resuelvan contra la geometría existente.
// ─────────────────────────────────────────────────────────────────
class Agent {
protected:
    Vector3 position;
    Gene gene;

public:
    Agent(const Vector3& position, const Gene& gene)
        : position(position), gene(gene) {}
    virtual ~Agent() = default;
    int energy = 0;

    void grow();
};

#endif // SM64_PORT_AGENT_H