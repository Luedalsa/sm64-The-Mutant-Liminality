//
// Created by Luis Alvarez on 16/09/2026.
//

#include "StructuralEdge.h"

#include <cassert>

bool BiologicalAbomination::ranAbiogenesis = false;
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
std::queue<ARN> Ribosome::mitosisPlans;
std::vector<StructuralEdge*> BiologicalAbomination::cellBorders;

void BiologicalAbomination::abiogenesis(StructuralEdge *&edge) {
    assert(!ranAbiogenesis
           && "BiologicalAbomination is still a singleton, Abiogenesis can only be run once.");
    ranAbiogenesis = true;
    // infiniteTravelInvariant = new InfiniteTravelInvariant(itemCount);

    std::cout << "PELIGRO: aquí comienza un estado geométrico inválido, ningún hilo debe acceder a "
                 "esta memoria mientras se ejecuta la abiogénesis, ya que se están creando aristas "
                 "estructurales sin propiedades definidas y se están violando las invariantes. Esto es "
                 "un estado temporal y controlado."
              << std::endl;

    auto *A = new StructuralEdge(*edge);
    auto *B = new StructuralEdge(*A);
    auto *C = new StructuralEdge(*A);
    auto *D = new StructuralEdge(*A);
    auto *E = new StructuralEdge(*A);
    auto *F = new StructuralEdge(*A);

    // itemCount.set(itemCount.value.value + 6);

    cellBorders.push_back(A);
    cellBorders.push_back(B);
    cellBorders.push_back(C);
    cellBorders.push_back(D);
    cellBorders.push_back(E);
    cellBorders.push_back(F);

    auto *sA = new StructuralFace();
    auto *sB = new StructuralFace();
    auto *sC = new StructuralFace();
    auto *sD = new StructuralFace();

    auto *lA = new BoundaryLoop{ *sA, A->halfEdges[0] };
    auto *lB = new BoundaryLoop{ *sB, B->halfEdges[1] };
    auto *lC = new BoundaryLoop{ *sC, C->halfEdges[1] };
    auto *lD = new BoundaryLoop{ *sD, A->halfEdges[1] };

    auto link = [](HalfEdge &h, HalfEdge &next, BoundaryLoop *loop) {
        h.nextEdge = &next;
        h.boundaryLoop = loop;
    };

    // Cara A: A0 → B0 → C0
    link(A->halfEdges[0], B->halfEdges[0], lA);
    link(B->halfEdges[0], C->halfEdges[0], lA);
    link(C->halfEdges[0], A->halfEdges[0], lA);
    // Cara B: B1 → E0 → D0
    link(B->halfEdges[1], E->halfEdges[0], lB);
    link(E->halfEdges[0], D->halfEdges[0], lB);
    link(D->halfEdges[0], B->halfEdges[1], lB);
    // Cara C: C1 → D1 → F0
    link(C->halfEdges[1], D->halfEdges[1], lC);
    link(D->halfEdges[1], F->halfEdges[0], lC);
    link(F->halfEdges[0], C->halfEdges[1], lC);
    // Cara D: A1 → F1 → E1
    link(A->halfEdges[1], F->halfEdges[1], lD);
    link(F->halfEdges[1], E->halfEdges[1], lD);
    link(E->halfEdges[1], A->halfEdges[1], lD);

    std::cout
        << "Fin de la abiogénesis, se han creado 6 aristas estructurales y 4 superficies estructurales."
        << std::endl;

    edge = A;
    //  FIXME other properties are UB, do not access before inheriting
}
void BiologicalAbomination::cellWallGrow(HalfEdge &halfEdgeA, HalfEdge &halfEdgeB,
                                         std::vector<StructuralEdge *> &outStructuralEdges,
                                         StructuralFace *&outNewSurface) {
    /*auto newEdge = new StructuralEdge(*surfaces.first, *surfaces.second, *nextEdges.first,
    *nextEdges.second); newEdge->idealSeparation = idealSeparation; newEdge->stableSeparation =
    stableSeparation; newEdge->position = position;
    newEdge->position.setPriority(ConstraintSolver::currentFrame); // TODO porfa encuentra el
    equivalente lógico a esto antes de borrar esta sección newEdge->relation = relation;*/

    std::cout << "PELIGRO: aquí comienza un estado geométrico inválido, ningún hilo debe acceder a "
                 "esta memoria mientras se ejecuta la mitosis, ya que se están creando aristas "
                 "estructurales sin propiedades definidas y se están violando las invariantes. Esto es "
                 "un estado temporal y controlado."
              << std::endl;

    auto newEdge =
        new StructuralEdge(halfEdgeA.structuralEdge); // TODO leak, use pool but idc about leaks rn its
                                                      // the least important thing here

    int ca = 0;
    int cb = 0;
    HalfEdge *h = &halfEdgeA;
    do {
        ca++;
        h = h->nextEdge;
    } while (h != &halfEdgeA);
    h = &halfEdgeB;
    do {
        cb++;
        h = h->nextEdge;
    } while (h != &halfEdgeB);
    assert(ca >= 3 && cb >= 3
           && "Cannot grow cell wall with less than 3 edges in either half edge loop.");

    // TODO here goes the assert to check if both half edges share vertex

    HalfEdge *originalNext = halfEdgeA.nextEdge;
    halfEdgeA.nextEdge = &newEdge->halfEdges[0];
    newEdge->halfEdges[0].nextEdge = originalNext;
    newEdge->halfEdges[0].boundaryLoop = halfEdgeA.boundaryLoop;

    originalNext = halfEdgeB.nextEdge;
    halfEdgeB.nextEdge = &newEdge->halfEdges[1];
    newEdge->halfEdges[1].nextEdge = originalNext;
    newEdge->halfEdges[1].boundaryLoop = halfEdgeB.boundaryLoop;

    outStructuralEdges.push_back(newEdge);

    // TODO if surfaces of both half edges are adjacent create an extra edge and extra surface to avoid
    // < 3 degree node.
/*
    auto newEdge2 = new StructuralEdge(halfEdgeB.structuralEdge);
    outStructuralEdges.push_back(newEdge2);

    outNewSurface = new StructuralFace();

    originalNext = halfEdgeA.nextEdge;
    halfEdgeA.nextEdge = &newEdge2->halfEdges[0];
    newEdge2->halfEdges[0].nextEdge = originalNext->nextEdge->nextEdge;
    originalNext->nextEdge->nextEdge = &newEdge2->halfEdges[1];
    newEdge2->halfEdges[1].nextEdge = originalNext;
    newEdge2->halfEdges[0].boundaryLoop = halfEdgeA.boundaryLoop;
    newEdge2->halfEdges[1].boundaryLoop = new BoundaryLoop{ *outNewSurface, newEdge2->halfEdges[1] };*/

    std::cout << "Fin de la mitosis, se han creado 2 aristas estructurales y 1 superficie estructural."
              << std::endl;
}
void BiologicalAbomination::inheritRelation(StructuralEdge &fromEdge,
                                            StructuralEdge &toEdge) { // rethink, nascent idea
    // toEdge.proportionalRelation = fromEdge.proportionalRelation;
}
void BiologicalAbomination::reWeight() { // rethink, nascent idea
    ConstraintSolver::checkAndSolve();
}
void BiologicalAbomination::inject(ARN &arn) {
    Ribosome::addMitosisPlan(arn);
}
void BiologicalAbomination::grow() {
    // auto edge = Ribosome::produce();
    //  TODO mitosis logic here
}