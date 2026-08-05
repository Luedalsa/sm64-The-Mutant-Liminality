//
// Created by Luis Alvarez on 04/08/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_PRISM_H
#define SM64_THE_MUTANT_LIMINALITY_PRISM_H

#include "Primitives.h"
#include "TriangleTypes.h"
#include "Gene.h"

#include <vector>

enum class PrismShape {
    Quad,
    Hexagon,
    Octagon,
    Misc
};

struct PrismParams {
    Vector3 center = Vector3::zero();
    float radius = 1500.0f;
    float baseHeight = 0.0f;
    float topHeight  = 1200.0f;
    int sides = 8;
    TriangleFaceType floorType = TriangleFaceType::CheckerboardFloor;
    TriangleFaceType roofType  = TriangleFaceType::CheckerboardFloor;
    TriangleFaceType wallType  = TriangleFaceType::InsideWallFirstFloor;
};

class Prism {
public:
    static std::vector<int> generate(const PrismParams& params);

    static PrismShape chooseShape();

    static int sidesForShape(PrismShape shape);
};

#endif // SM64_THE_MUTANT_LIMINALITY_PRISM_H