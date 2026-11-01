//
// Created by Luis Alvarez on 16/09/2026.
//

#include "StructuralEdge.h"

#include <cassert>

HalfEdge::HalfEdge(StructuralEdge &e) : /*semanticHalfEdge(), */ structuralEdge(e) {
}
void HalfEdge::linkBoundaryLoop(BoundaryLoop &loop) {
    assert(boundaryLoop == nullptr
        && "HalfEdge linkBoundaryLoop() called on a HalfEdge that already has a boundaryLoop. "
           "This link is immutable once set.");
    if (boundaryLoop != nullptr) {
        throw NonRecoverableContradictionException(
            "HalfEdge already has a boundaryLoop and its link is immutable.");
    }
    boundaryLoop = &loop;
}
BoundaryLoop &HalfEdge::getBoundaryLoop() const {
    assert(boundaryLoop != nullptr
        && "HalfEdge getBoundaryLoop() called on a HalfEdge with null boundaryLoop. This should not "
           "happen, as HalfEdges are managed by the system and should always have a valid "
           "boundaryLoop.");
    if (boundaryLoop == nullptr) {
        throw NonRecoverableContradictionException(
            "HalfEdge getBoundaryLoop() called on a HalfEdge with null boundaryLoop.");
    }
    return *boundaryLoop;
}
HalfEdge &HalfEdge::walk() const {
    assert(nextEdge != nullptr
        && "HalfEdge walk() called on a HalfEdge with null nextEdge. This should not happen, as "
           "HalfEdges are managed by the system and should always have a valid nextEdge.");
    if (nextEdge == nullptr) {
        throw NonRecoverableContradictionException(
            "HalfEdge walk() called on a HalfEdge with null nextEdge.");
    }
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
    return -std::numbers::pi / 2.0f; // TODO: calcular separación angular entre dos medias aristas estructurales en 3D
}
StructuralEdge::StructuralEdge() : halfEdges{ HalfEdge(*this), HalfEdge(*this) } {
}
bool StructuralEdge::isLinked() const {
    return halfEdges[0].isLinked() && halfEdges[1].isLinked();
}
void StructuralEdge::constrain() {
    assert(!normalSeparationConstraint.has_value() && !directionVerticesRelationSizeConstraint.has_value()
        && "StructuralEdge constrain() called on a StructuralEdge that already has constraints.");
    if (normalSeparationConstraint.has_value() || directionVerticesRelationSizeConstraint.has_value()) {
        throw NonRecoverableContradictionException(
            "StructuralEdge already has constraints and they can only be created once.");
    }
    // Los enlaces ya están completos: las superficies y la arista siguiente son accesibles.
    normalSeparationConstraint.emplace(
        DirectionAndSeparation(sphericalDirection[0], sphericalDirection[1], stableSeparation, 100),
        SphericalDirection(halfEdges[0].getBoundaryLoop().getSurface().normal[0],
                           halfEdges[0].getBoundaryLoop().getSurface().normal[1], 1),
        SphericalDirection(halfEdges[1].getBoundaryLoop().getSurface().normal[0],
                           halfEdges[1].getBoundaryLoop().getSurface().normal[1], 100));
    directionVerticesRelationSizeConstraint.emplace(
        SphericalCoordinates(sphericalDirection[0], sphericalDirection[1], stableSize, 100),
        VVector3(displayVertex.position[0], displayVertex.position[1], displayVertex.position[2], 100),
        VVector3(halfEdges[0].walk().structuralEdge.displayVertex.position[0], halfEdges[0].walk().structuralEdge.displayVertex.position[1], halfEdges[0].walk().structuralEdge.displayVertex.position[2], 100)); // TODO Revisar de donde sale el segundo vértice
}
StructuralEdge::~StructuralEdge() {
    assert(false
           && "StructuralEdge destructor called. This should not happen, as StructuralEdges are "
              "managed by the system and should not be deleted manually.");
}
HalfEdge &StructuralEdge::getHalfEdge(int index) {
    if (index < 0 || index > 1) {
        throw std::out_of_range("HalfEdge index must be 0 or 1.");
    }
    return halfEdges[index];
}
std::queue<ARN> Ribosome::mitosisPlans;