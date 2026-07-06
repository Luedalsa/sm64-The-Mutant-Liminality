//
// Created by Luis Alvarez on 05/07/2026.
//

#include "Primitives.h"
#include "TriangleCollection.h"
#include "VertexCollection.h"

Vector3 SurfaceTransform::getUVPosition() const {
    AbstractTriangle* tri = TriangleCollection::getTriangle(triangle);
    const Vector3& A = VertexCollection::getVertex(tri->getVertex(0))->position;
    const Vector3& B = VertexCollection::getVertex(tri->getVertex(1))->position;
    const Vector3& C = VertexCollection::getVertex(tri->getVertex(2))->position;

    float w = 1.0f - u - v;
    return A * w + B * u + C * v;
}