//
// Created by Luis Alvarez on 19/06/2026.
//

#ifndef SM64_PORT_AGENT_H
#define SM64_PORT_AGENT_H
#include "VertexCollection.h"
#include "TriangleCollection.h"
#include "AbstractVertex.h"
#include "Primitives.h"

extern "C" {
#include "engine/math_util.h"
}

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#include <vector>

class Agent {
public:
    Agent(SurfaceTransform transform) {
    }
    virtual ~Agent() = default;
    int energy = 0;

  virtual void grow() = 0;
};

class DebugAgent : public Agent {
public:
    DebugAgent(const SurfaceTransform &transform) : Agent(transform) {energy = 3;};
    ~DebugAgent() override = default;
    void grow() override {
        energy--;
        // --- VÉRTICES ---
        // Base (Y = 0)
        int v4 = VertexCollection::createVertex(-3000, 0, -3000); // Frente, Izquierda
        int v2 = VertexCollection::createVertex(3000, 0, -3000);  // Frente, Derecha
        int v3 = VertexCollection::createVertex(-3000, 0, 3000);  // Atrás, Izquierda
        int v1 = VertexCollection::createVertex(3000, 0, 3000);   // Atrás, Derecha

        // Techo (Y = 3000)
        int v6 = VertexCollection::createVertex(-3000, 3000, -3000); // Frente, Izquierda (Asignado)
        int v5 = VertexCollection::createVertex(3000, 3000, -3000);  // Frente, Derecha
        int v7 = VertexCollection::createVertex(-3000, 3000, 3000);  // Atrás, Izquierda (Asignado)
        int v8 = VertexCollection::createVertex(3000, 3000, 3000);   // Atrás, Derecha (Asignado)

        // --- TRIÁNGULOS ---
        // Cara Inferior (Base)
        TriangleCollection::createTriangle(v1, v2, v3);
        TriangleCollection::createTriangle(v3, v2, v4);

        // Cara Frontal (Z = -3000)
        TriangleCollection::createTriangle(v4, v2, v5);
        TriangleCollection::createTriangle(v4, v5, v6);

        // Cara Superior (Techo)
        TriangleCollection::createTriangle(v6, v5, v8);
        TriangleCollection::createTriangle(v6, v8, v7);

        // Cara Trasera (Z = 3000)
        TriangleCollection::createTriangle(v1, v3, v7);
        TriangleCollection::createTriangle(v1, v7, v8);

        // Cara Izquierda (X = -3000)
        TriangleCollection::createTriangle(v3, v4, v6);
        TriangleCollection::createTriangle(v3, v6, v7);

        // Cara Derecha (X = 3000)
        TriangleCollection::createTriangle(v2, v1, v8);
        TriangleCollection::createTriangle(v2, v8, v5);

    };
};

#endif // SM64_PORT_AGENT_H
