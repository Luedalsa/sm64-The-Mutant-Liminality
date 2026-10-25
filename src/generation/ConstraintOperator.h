//
// Created by Luis Alvarez on 24/10/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_CONSTRAINTOPERATOR_H
#define SM64_THE_MUTANT_LIMINALITY_CONSTRAINTOPERATOR_H

#include "StructuralEdge.h"

// La cantidad de operadores debe ser el mínimo posible

struct StructuralEdge;
// Mudando a operadores b-rep Euler
// Operadores topológicos: mitosis, fold
// Operadores semánticos: heredar de otra arista, relaciones, proporciones, angulos, textura
// operadores propagadores: resuelven mediante ecuaciones de las restricciones a traves de las variable y scalar, propagando cambios a otras aristas y superficies
// Invariante: La geometría se renderiza entre cada operación, por lo que los operadores deben recibir geometría renderizable y devolver geometría renderizable.
// Invariante: la geometría es un grafo de grado >=3, cada uno de los vértices de la geometría debe estar conectado a más de 2 aristas, y cada arista debe estar conectada a exactamente 2 superficies. La estructura de datos y los operadores deben reflejarla
// La topología es una esfera, los toros están fuera del alcance del proyecto por que no tienen apariciones relevantes en la topología de Mario 64
// Nueva idea: tal vez debería hacer un corte boundary loop en la geometría semántica y garantizar su conexión topológicamente correcta mediante un contrato, ambas partes pueden ser modificadas para encajar, aún si violan la separación angular por parte de la semántica
// Se basaría en las diferencias de semántica de medias aristas para decidir hasta donde cortar, se preferirá bordear las aristas semánticas con alta diferencia semántica, mientras que se preferirá incluir medias aristas con baja diferencia
// Mientras haya fidelidad arquitectónica baja, o la habitación sea demasiado pequeña, se preferirá cortar un trozo más grande de la semántica. No estoy seguro de que vocabulario usar para describirlo.
 // Se preferirá hacer cortes cuyas medias aristas
// semánticas involucren la menor cantidad de superficies posibles (mínimo 3 para preservar la
// invariante de vértices grado >= 3)
struct ConstraintOperator {
    static bool ranAbiogenesis;
    static std::vector<StructuralEdge*> cellBorders;
    static ItemCount itemCount/* = Scalar<ItemCount>()*/;
    static InfiniteTravelInvariant* infiniteTravelInvariant;

    // genera nuevas aristas estructurales desde memoria basura y devuelve una arista estructural (crea vida a partir de materia inerte)
    // TODO cubrir todas las propiedades
    static void abiogenesis(StructuralEdge *& edge);

    // La posición del nuevo edge siempre será indefinida, activará un TotallyUnrepresentableState.
    // Los structural edges de salida serán promesas que heredarán semántica, no están listos para ser usados.
    static void cellWallGrow(HalfEdge& halfEdgeA, HalfEdge& halfEdgeB, std::vector<StructuralEdge*>& outStructuralEdges, StructuralFace*& outNewSurface);
    /*  Merging this code with mitosis.
        static void fold(HalfEdge& edgeA, HalfEdge& edgeB, StructuralEdge*& outStructuralEdge,
       StructuralFace*& outStructuralFace) { StructuralFace* newSurface = new StructuralFace();
            StructuralEdge* newEdge = new StructuralEdge(edgeA.structuralEdge); // TODO complete

            if (&edgeA.surface != &edgeB.surface) {
                throw NonRecoverableContradictionException("Cannot fold edges from different
       surfaces.");
            }

            HalfEdge* h = &edgeA;

            while (h != &edgeB) {
                h->surface = newSurface;
                h = h->nextEdge;
            }
            edgeB.surface = newSurface;

            outStructuralEdge = newEdge;
            outStructuralFace = newSurface;
        }*/

    static void inheritRelation(StructuralEdge& fromEdge, StructuralEdge& toEdge);

    static void reWeight();

    static void inject(ARN& arn);

    static void grow();

    static void applyBestOperator() {
        assert(!Diagnosable::getViolatedConstraints().empty() && "ConstraintSolver: No constraints to solve, Generator reached a deadlock so it can't generate more geometry, violating invariant of infinite travel.");
        reWeight();
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_CONSTRAINTOPERATOR_H
