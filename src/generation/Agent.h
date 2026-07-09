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
        up = Vector3(0, 1, 0);

        Vector3 inheritedForward = birthPosition.forward;
        Vector3 flatForward(inheritedForward.x, 0, inheritedForward.z);

        Vector3 reference = (flatForward.length() > 0.0001f)
                           ? flatForward.normalized()
                           : Vector3(0, 0, -1);

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
    void grow() override;
};

#endif // SM64_PORT_AGENT_H
