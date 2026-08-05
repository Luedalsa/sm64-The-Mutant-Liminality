//
// Created by Luis Alvarez on 04/08/2026.
//

#include "Prism.h"
#include "VertexCollection.h"
#include "TriangleCollection.h"

#include <cmath>
#include <cstdint>

namespace {

constexpr float kPi = 3.14159265358979323846f;

} // namespace

PrismShape Prism::chooseShape() {
    float roll = static_cast<float>(rand()) / static_cast<float>(INT_MAX);

    if (roll < 0.45f) return PrismShape::Octagon;
    if (roll < 0.85f) return PrismShape::Quad;
    if (roll < 0.97f) return PrismShape::Hexagon;
    return PrismShape::Misc;
}

int Prism::sidesForShape(PrismShape shape) {
    switch (shape) {
        case PrismShape::Quad:    return 4;
        case PrismShape::Hexagon: return 6;
        case PrismShape::Octagon: return 8;
        case PrismShape::Misc:    return 5;
    }
    return 8;
}

std::vector<int> Prism::generate(const PrismParams& params) {
    std::vector<int> createdTriangles;
    const int sides = params.sides;

    std::vector<Vector3> bottom(sides), top(sides);
    for (int i = 0; i < sides; ++i) {
        float angle = kPi * 2.0f * static_cast<float>(i) / static_cast<float>(sides);
        float x = params.center.x + params.radius * std::cos(angle);
        float z = params.center.z + params.radius * std::sin(angle);
        bottom[i] = Vector3(x, params.baseHeight, z);
        top[i]    = Vector3(x, params.topHeight, z);
    }

    std::vector<int> bottomIdx(sides), topIdx(sides);
    for (int i = 0; i < sides; ++i) {
        bottomIdx[i] = VertexCollection::createVertex(bottom[i]);
        topIdx[i]    = VertexCollection::createVertex(top[i]);
    }

    for (int i = 1; i < sides - 1; ++i) {
        createdTriangles.push_back(TriangleCollection::createTriangle(
            bottomIdx[0], bottomIdx[i + 1], bottomIdx[i], params.floorType));
    }

    for (int i = 1; i < sides - 1; ++i) {
        createdTriangles.push_back(TriangleCollection::createTriangle(
            topIdx[0], topIdx[i], topIdx[i + 1], params.roofType));
    }

    for (int i = 0; i < sides; ++i) {
        int next = (i + 1) % sides;
        createdTriangles.push_back(TriangleCollection::createTriangle(
            bottomIdx[i], bottomIdx[next], topIdx[next], params.wallType));
        createdTriangles.push_back(TriangleCollection::createTriangle(
            bottomIdx[i], topIdx[next], topIdx[i], params.wallType));
    }

    return createdTriangles;
}