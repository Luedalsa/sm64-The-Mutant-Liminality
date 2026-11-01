//
// Created by Luis Alvarez on 24/10/2026.
//

#include "ConstraintOperator.h"
#include "StagedBatch.h"

bool ConstraintOperator::ranAbiogenesis = false;
std::vector<StructuralEdge*> ConstraintOperator::cellBorders;

ItemCount ConstraintOperator::itemCount = ItemCount(INT_MAX);
InfiniteTravelInvariant* ConstraintOperator::infiniteTravelInvariant = new InfiniteTravelInvariant(itemCount);

void ConstraintOperator::abiogenesis(StructuralEdge *&edge) {
    assert(!ranAbiogenesis
           && "BiologicalAbomination is still a singleton, Abiogenesis can only be run once.");
    ranAbiogenesis = true;

    std::cout << "PELIGRO: aquí comienza un estado geométrico inválido, ningún hilo debe acceder a "
                 "esta memoria mientras se ejecuta la abiogénesis, ya que se están creando aristas "
                 "estructurales sin enlazar ni restricciones. Esto es un estado temporal y controlado: "
                 "termina cuando el lote completa sus etapas de enlace y restricciones."
              << std::endl;

    StagedBatch<StructuralEdge, HalfEdge, BoundaryLoop, StructuralFace> batch;

    // Etapa 1: Instancia. Los objetos nacen con propiedades por defecto, sin enlaces ni restricciones.
    auto *A = new StructuralEdge();
    auto *B = new StructuralEdge();
    auto *C = new StructuralEdge();
    auto *D = new StructuralEdge();
    auto *E = new StructuralEdge();
    auto *F = new StructuralEdge();

    itemCount.set(6);

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

    auto *lA = new BoundaryLoop();
    auto *lB = new BoundaryLoop();
    auto *lC = new BoundaryLoop();
    auto *lD = new BoundaryLoop();

    for (auto *e : {A, B, C, D, E, F}) {
        batch.add(*e);
        batch.add(e->halfEdges[0]);
        batch.add(e->halfEdges[1]);
    }
    for (auto *s : {sA, sB, sC, sD}) {
        batch.add(*s);
    }
    for (auto *l : {lA, lB, lC, lD}) {
        batch.add(*l);
    }

    // Etapa 2: Enlace. Todos los enlaces mutuos se establecen aquí, una sola vez, y el lote
    // comprueba que ningún participante quedó sin enlazar antes de permitir las restricciones.
    batch.link([&] {
        lA->link(*sA, A->halfEdges[0]);
        lB->link(*sB, B->halfEdges[1]);
        lC->link(*sC, C->halfEdges[1]);
        lD->link(*sD, A->halfEdges[1]);

        sA->linkOuterLoop(*lA);
        sB->linkOuterLoop(*lB);
        sC->linkOuterLoop(*lC);
        sD->linkOuterLoop(*lD);

        auto linkHalfEdge = [](HalfEdge &h, HalfEdge &next, BoundaryLoop *loop) {
            h.nextEdge = &next;
            h.linkBoundaryLoop(*loop);
        };

        // Cara A: A0 → B0 → C0
        linkHalfEdge(A->halfEdges[0], B->halfEdges[0], lA);
        linkHalfEdge(B->halfEdges[0], C->halfEdges[0], lA);
        linkHalfEdge(C->halfEdges[0], A->halfEdges[0], lA);
        // Cara B: B1 → E0 → D0
        linkHalfEdge(B->halfEdges[1], E->halfEdges[0], lB);
        linkHalfEdge(E->halfEdges[0], D->halfEdges[0], lB);
        linkHalfEdge(D->halfEdges[0], B->halfEdges[1], lB);
        // Cara C: C1 → D1 → F0
        linkHalfEdge(C->halfEdges[1], D->halfEdges[1], lC);
        linkHalfEdge(D->halfEdges[1], F->halfEdges[0], lC);
        linkHalfEdge(F->halfEdges[0], C->halfEdges[1], lC);
        // Cara D: A1 → F1 → E1
        linkHalfEdge(A->halfEdges[1], F->halfEdges[1], lD);
        linkHalfEdge(F->halfEdges[1], E->halfEdges[1], lD);
        linkHalfEdge(E->halfEdges[1], A->halfEdges[1], lD);
    });

    // Etapa 3: Restricciones. Con los enlaces completos, las aristas crean sus constraints.
    batch.constrain();

    std::cout
        << "Fin de la abiogénesis, se han creado 6 aristas estructurales y 4 superficies estructurales."
        << std::endl;

    edge = A;
    //  FIXME other properties are UB, do not access before inheriting
}
void ConstraintOperator::cellWallGrow(HalfEdge &halfEdgeA, HalfEdge &halfEdgeB,
                                         std::vector<StructuralEdge *> &outStructuralEdges,
                                         StructuralFace *&outNewSurface) {
    /*auto newEdge = new StructuralEdge(*surfaces.first, *surfaces.second, *nextEdges.first,
    *nextEdges.second); newEdge->idealSeparation = idealSeparation; newEdge->stableSeparation =
    stableSeparation; newEdge->position = position;
    newEdge->position.setPriority(ConstraintSolver::currentFrame); // TODO porfa encuentra el
    equivalente lógico a esto antes de borrar esta sección newEdge->relation = relation;*/

    std::cout << "PELIGRO: aquí comienza un estado geométrico inválido, ningún hilo debe acceder a "
                 "esta memoria mientras se ejecuta la mitosis, ya que se están creando aristas "
                 "estructurales sin enlazar ni restricciones y se están reempalmando anillos de "
                 "medias aristas. Esto es un estado temporal y controlado: termina cuando el lote "
                 "completa sus etapas de enlace y restricciones."
              << std::endl;

    // Solo los objetos nuevos entran en el lote; los HalfEdge existentes ya estaban enlazados.
    StagedBatch<StructuralEdge, HalfEdge, BoundaryLoop, StructuralFace> batch;

    // Etapa 1: Instancia.
    auto newEdge = new StructuralEdge(); // TODO leak, use pool but idc about leaks rn its
                                         // the least important thing here
    auto newEdge2 = new StructuralEdge();
    outNewSurface = new StructuralFace();
    auto *newLoop = new BoundaryLoop();

    for (auto *e : {newEdge, newEdge2}) {
        batch.add(*e);
        batch.add(e->halfEdges[0]);
        batch.add(e->halfEdges[1]);
    }
    batch.add(*outNewSurface);
    batch.add(*newLoop);

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

    // Etapa 2: Enlace.
    batch.link([&] {
        HalfEdge *originalNext = halfEdgeA.nextEdge;
        halfEdgeA.nextEdge = &newEdge->halfEdges[0];
        newEdge->halfEdges[0].nextEdge = originalNext;
        newEdge->halfEdges[0].linkBoundaryLoop(halfEdgeA.getBoundaryLoop());

        originalNext = halfEdgeB.nextEdge;
        halfEdgeB.nextEdge = &newEdge->halfEdges[1];
        newEdge->halfEdges[1].nextEdge = originalNext;
        newEdge->halfEdges[1].linkBoundaryLoop(halfEdgeB.getBoundaryLoop());

        // TODO if surfaces of both half edges are adjacent create an extra edge and extra surface to avoid
        // < 3 degree node.

        originalNext = halfEdgeA.nextEdge;
        halfEdgeA.nextEdge = &newEdge2->halfEdges[0];
        newEdge2->halfEdges[0].nextEdge = originalNext->nextEdge->nextEdge;
        originalNext->nextEdge->nextEdge = &newEdge2->halfEdges[1];
        newEdge2->halfEdges[1].nextEdge = originalNext;
        newEdge2->halfEdges[0].linkBoundaryLoop(halfEdgeA.getBoundaryLoop());
        newLoop->link(*outNewSurface, newEdge2->halfEdges[1]);
        outNewSurface->linkOuterLoop(*newLoop);
        newEdge2->halfEdges[1].linkBoundaryLoop(*newLoop);
    });

    outStructuralEdges.push_back(newEdge);
    outStructuralEdges.push_back(newEdge2);

    // Etapa 3: Restricciones.
    batch.constrain();

    std::cout << "Fin de la mitosis, se han creado 2 aristas estructurales y 1 superficie estructural."
              << std::endl;
}
void ConstraintOperator::inheritRelation(StructuralEdge &fromEdge,
                                            StructuralEdge &toEdge) { // rethink, nascent idea
    // toEdge.proportionalRelation = fromEdge.proportionalRelation;
}
void ConstraintOperator::reWeight() { // rethink, nascent idea
    ConstraintSolver::checkAndSolve();
}
void ConstraintOperator::inject(ARN &arn) {
    Ribosome::addMitosisPlan(arn);
}
void ConstraintOperator::grow() {
    // auto edge = Ribosome::produce();
    //  TODO mitosis logic here
}