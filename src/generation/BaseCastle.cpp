//
// Created by Luis Alvarez on 15/08/2026.
//

#include "BaseCastle.h"
#include "VertexCollection.h"

// Índices:
//   0: (-x, 0,    -z)   4: (-x, 2000, -z)
//   1: (+x, 0,    -z)   5: (+x, 2000, -z)
//   2: (-x, 0,    +z)   6: (-x, 2000, +z)
//   3: (+x, 0,    +z)   7: (+x, 2000, +z)
int BaseCastle::vertices[8] = {
    VertexCollection::createVertex(Vector3(-1000, 0, -1000)),
    VertexCollection::createVertex(Vector3(1000, 0, -1000)),
    VertexCollection::createVertex(Vector3(-1000, 0, 1000)),
    VertexCollection::createVertex(Vector3(1000, 0, 1000)),
    VertexCollection::createVertex(Vector3(-1000, 2000, -1000)),
    VertexCollection::createVertex(Vector3(1000, 2000, -1000)),
    VertexCollection::createVertex(Vector3(-1000, 2000, 1000)),
    VertexCollection::createVertex(Vector3(1000, 2000, 1000))
};

// Todas las normales apuntan HACIA EL INTERIOR de la caja (mismo criterio
// que Prism::generate: piso +Y, techo -Y, paredes hacia el centro).
// Winding verificado a mano triángulo por triángulo, no asumido.
AbstractTriangle BaseCastle::baseTriangles[12] = {
    // Piso (y=0), normal +Y
    AbstractTriangle(BaseCastle::vertices[0], BaseCastle::vertices[2], BaseCastle::vertices[3],
        TriangleFaceType::CheckerboardFloor),
    AbstractTriangle(BaseCastle::vertices[0], BaseCastle::vertices[3], BaseCastle::vertices[1],
        TriangleFaceType::CheckerboardFloor),

    // Techo (y=2000), normal -Y
    AbstractTriangle(BaseCastle::vertices[4], BaseCastle::vertices[7], BaseCastle::vertices[6],
        TriangleFaceType::InsideRoofFirstFloor),
    AbstractTriangle(BaseCastle::vertices[4], BaseCastle::vertices[5], BaseCastle::vertices[7],
        TriangleFaceType::InsideRoofFirstFloor),

    // Pared trasera (z=-1000), normal +Z
    AbstractTriangle(BaseCastle::vertices[0], BaseCastle::vertices[1], BaseCastle::vertices[5],
        TriangleFaceType::InsideWallFirstFloor),
    AbstractTriangle(BaseCastle::vertices[0], BaseCastle::vertices[5], BaseCastle::vertices[4],
        TriangleFaceType::InsideWallFirstFloor),

    // Pared frontal (z=+1000), normal -Z
    AbstractTriangle(BaseCastle::vertices[2], BaseCastle::vertices[7], BaseCastle::vertices[3],
        TriangleFaceType::InsideWallFirstFloor),
    AbstractTriangle(BaseCastle::vertices[2], BaseCastle::vertices[6], BaseCastle::vertices[7],
        TriangleFaceType::InsideWallFirstFloor),

    // Pared izquierda (x=-1000), normal +X
    AbstractTriangle(BaseCastle::vertices[0], BaseCastle::vertices[4], BaseCastle::vertices[2],
        TriangleFaceType::InsideWallFirstFloor),
    AbstractTriangle(BaseCastle::vertices[4], BaseCastle::vertices[6], BaseCastle::vertices[2],
        TriangleFaceType::InsideWallFirstFloor),

    // Pared derecha (x=+1000), normal -X
    AbstractTriangle(BaseCastle::vertices[1], BaseCastle::vertices[3], BaseCastle::vertices[5],
        TriangleFaceType::InsideWallFirstFloor),
    AbstractTriangle(BaseCastle::vertices[3], BaseCastle::vertices[7], BaseCastle::vertices[5],
        TriangleFaceType::InsideWallFirstFloor),
};