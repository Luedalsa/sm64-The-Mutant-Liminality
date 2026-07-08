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

#include <cmath>

class Agent {

    void getLocalBasis(Vector3& right, Vector3& up, Vector3& forward) const {
        up = TriangleCollection::getTriangle(birthPosition.triangle)->getNormal();

        Vector3 inheritedForward = birthPosition.forward;
        Vector3 reference = (std::fabs(up.dot(inheritedForward.normalized())) < 0.99f)
                           ? inheritedForward
                           : Vector3(0, 1, 0);

        right   = reference.cross(up).normalized();
        forward = right.cross(up).normalized();
    }
protected:
    SurfaceTransform birthPosition;

public:
    explicit Agent(const SurfaceTransform& transform) : birthPosition(transform) {}
    virtual ~Agent() = default;
    int energy = 0;

    virtual void grow() = 0;

    int createLocalVertex(Vector3 v) {
        Vector3 right, up, forward;
        getLocalBasis(right, up, forward);

        Vector3 worldPos = birthPosition.getUVPosition()
                          + right   * v.x
                          + up      * v.y
                          + forward * v.z;

        return VertexCollection::createVertex(worldPos);
    }
};

class DebugAgent : public Agent {
public:
    DebugAgent(const SurfaceTransform &transform) : Agent(transform) {energy = 3;};
    ~DebugAgent() override = default;
    void grow() override {
        energy--;

        float fy = birthPosition.forward.y;
        constexpr float EPS = 0.001f;
        float cosPitch = std::sqrt(std::max(0.0f, 1.0f - fy * fy));
        float frontHeight = (cosPitch > EPS) ? (fy * 3000.0f / cosPitch) : std::copysign(30000.0f, fy);

        int v4 = createLocalVertex({-3000, frontHeight, -3000}); // Frente, Izquierda
        int v2 = createLocalVertex({3000, frontHeight, -3000});  // Frente, Derecha
        int v3 = createLocalVertex({-3000, 0, 0});               // Atrás, Izquierda
        int v1 = createLocalVertex({3000, 0, 0});                // Atrás, Derecha

        int v6 = createLocalVertex({-3000, frontHeight + 3000.0f, -3000}); // Frente, Izquierda
        int v5 = createLocalVertex({3000, frontHeight + 3000.0f, -3000});  // Frente, Derecha
        int v7 = createLocalVertex({-3000, 3000, 0});                      // Atrás, Izquierda
        int v8 = createLocalVertex({3000, 3000, 0});                       // Atrás, Derecha

        // --- TRIÁNGULOS ---
        // Cara Inferior (Base)

        TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v1, v2, v3);
        TriangleCollection::createTriangle<LavaTriangle>(v3, v2, v4);

        // Cara Frontal (Z = -3000)
        TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v4, v2, v5);
        TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v4, v5, v6);

        // Cara Superior (Techo)
        TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v6, v5, v8);
        TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v6, v8, v7);

        // Cara Trasera (Z = 3000)
        TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v1, v3, v7);
        TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v1, v7, v8);

        // Cara Izquierda (X = -3000)
        TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v3, v4, v6);
        TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v3, v6, v7);

        // Cara Derecha (X = 3000)
        TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v2, v1, v8);
        TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v2, v8, v5);


    };
};

#endif // SM64_PORT_AGENT_H
