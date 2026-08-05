//
// Created by Luis Alvarez on 19/06/2026.
//

#include "Agent.h"
#include "Prism.h"
#include "TriangleOverlapSolver.h"

namespace {

PrismParams makePrismParams(const Vector3& position, const Gene& gene, float radius, float height) {
    PrismShape shape = Prism::chooseShape();

    PrismParams params;
    params.center     = position;
    params.radius      = radius;
    params.baseHeight = rand()%500;
    params.topHeight  = height+rand()%500;
    params.sides       = Prism::sidesForShape(shape);
    params.floorType  = TriangleFaceType::CheckerboardFloor;
    params.roofType   = TriangleFaceType::InsideRoofFirstFloor;
    params.wallType   = TriangleFaceType::InsideWallFirstFloor;
    return params;
}

} // namespace

void Agent::grow() {
    energy--;

    PrismParams params = makePrismParams(position, gene, /*radius=*/1500.0f, /*height=*/1200.0f);
    std::vector<int> triangleIds = Prism::generate(params);
    TriangleOverlapSolver::resolve(triangleIds);
}
