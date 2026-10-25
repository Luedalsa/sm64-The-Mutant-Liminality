//
// Created by Luis Alvarez on 16/09/2026.
//

#include "StructuralEdge.h"

#include <cassert>

HalfEdge::HalfEdge(StructuralEdge &e, BoundaryLoop &s, HalfEdge &n)
    : boundaryLoop(&s), nextEdge(&n), /*semanticHalfEdge(), */ structuralEdge(e) {
}
BoundaryLoop &HalfEdge::getBoundaryLoop() const {
    assert(boundaryLoop != nullptr
        && "HalfEdge getBoundaryLoop() called on a HalfEdge with null boundaryLoop. This should not "
           "happen, as HalfEdges are managed by the system and should always have a valid "
           "boundaryLoop.");
    return *boundaryLoop;
}
HalfEdge &HalfEdge::walk() const {
    assert(nextEdge != nullptr
        && "HalfEdge walk() called on a HalfEdge with null nextEdge. This should not happen, as "
           "HalfEdges are managed by the system and should always have a valid nextEdge.");
    assert(nextEdge != this
        && "HalfEdge walk() called on a HalfEdge that points to itself as nextEdge. This should not "
           "happen, as HalfEdges are managed by the system and should always have a valid nextEdge.");
    assert(nextEdge->boundaryLoop == this->boundaryLoop
        && "HalfEdge walk() called on a HalfEdge whose nextEdge belongs to a different BoundaryLoop. "
           "This should not happen, as HalfEdges are managed by the system and should always have a "
           "valid nextEdge within the same BoundaryLoop.");
    return *nextEdge;
}
HalfEdge &HalfEdge::getEvilTwin() const {
    return (this == &structuralEdge.getHalfEdge(0)) ? structuralEdge.getHalfEdge(1)
                                                    : structuralEdge.getHalfEdge(0);
}
float HalfEdge::getAngleSeparation() const {
    HalfEdge* t = &getEvilTwin();
    do { t = &t->walk(); } while (t != &getEvilTwin());
    auto edgeFacesSeparation = t->structuralEdge.stableSeparation;
    return -M_PI / 2.0f; // TODO: calcular separación angular entre dos medias aristas estructurales en 3D
}
std::queue<ARN> Ribosome::mitosisPlans;