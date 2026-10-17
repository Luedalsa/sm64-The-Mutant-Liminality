//
// Created by Luis Alvarez on 16/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
#define SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
#include "BaseEdge.h"
#include "Constraintsolver.h"
#include "Diagnosable.h"
#include "Inherited.h"
#include "SemanticEdge.h"
#include "StructuralSurface.h"

#include <vector>

class StructuralSurface;
struct BiologicalAbomination;
struct BoundaryLoop;

class HalfEdge {
private:
    HalfEdge(StructuralEdge& e, BoundaryLoop& s, HalfEdge& n) : boundaryLoop(&s), nextEdge(&n), structuralEdge(e) {}

    BoundaryLoop* boundaryLoop;
    HalfEdge* nextEdge; // This value must NEVER be null
    friend struct StructuralEdge;
    friend struct BiologicalAbomination;
public:
    //Variable<bool> semanticallyPrecise = Variable(false, 0); I forgor what dis does
    TriangleFaceType stableSurfaceType;
    StructuralEdge& structuralEdge;

    HalfEdge& walk() const {
        return *nextEdge;
    }
};

// Nótese que StructuralEdge no tiene un constructor por defecto, ya que siempre debe estar asociado a dos superficies y dos edges siguientes. Esto garantiza que la estructura de la geometría sea consistente y evita la creación de edges huérfanos o mal definidos.
// Es una invariante del sistema que cada StructuralEdge esté correctamente conectado a sus superficies y edges adyacentes, tambien reduce las vulnerabilidades en la estructura de datos en runtime, no necesita asserts y reduce la carga cognitiva de mantener toda la arquitectura en mi memoria de trabajo al momento de establecer restricciones
struct StructuralEdge : BaseEdge, Diagnosable {
private:
    StructuralEdge(StructuralEdge & structuralEdge)
      : halfEdges{ HalfEdge(*this, *structuralEdge.halfEdges[0].boundaryLoop,
                            *structuralEdge.halfEdges[0].nextEdge),
                   HalfEdge(*this, *structuralEdge.halfEdges[1].boundaryLoop,
                            *structuralEdge.halfEdges[1].nextEdge) },
        surfaceTypes(structuralEdge.surfaceTypes),
        edgeLength(structuralEdge.edgeLength),
        lengthOverload(structuralEdge.lengthOverload),
        proportionalRelation(structuralEdge.proportionalRelation),
        position{ structuralEdge.position[0], structuralEdge.position[1], structuralEdge.position[2] },
        stableSeparation(structuralEdge.stableSeparation),
        semanticTension(structuralEdge.semanticTension) {

    }
    HalfEdge halfEdges[2];
    SemanticEdge* surfaceTypes = nullptr;
    SemanticEdge* edgeLength = nullptr;
    float lengthOverload = 0.0f;
    SemanticEdge* proportionalRelation = nullptr;
    SemanticEdge* semanticSeparation = nullptr;
    SemanticEdge* nearestSemantic = nullptr;
/*
    StructuralEdge(StructuralSurface& surfaceA, StructuralSurface& surfaceB, StructuralEdge& nextEdgeA, StructuralEdge& nextEdgeB)
        : halfEdges(HalfEdge(surfaceA, nextEdgeA), HalfEdge(surfaceB, nextEdgeB)) {*/
        /*auto c = new NormalSeparationConstraint({surfaceA.normal, surfaceB.normal}, stableSeparation);
        surfaceA.normal.observers.push_back(c);
        surfaceB.normal.observers.push_back(c);
        stableSeparation.observers.push_back(c);

        StructuralEdge* prevA = nextEdges.first;
        StructuralEdge* currA = nextEdges.first;
        StructuralEdge* prevB = nextEdges.second;
        StructuralEdge* currB = nextEdges.second;
        while (prevA != this) {
            prevA = currA;
            currA = currA->nextEdges.first;
        }
        while (prevB != this) {
            prevB = currB;
            currB = currB->nextEdges.second;
        }
        auto c2 = new VertexPositionConstraint({prevA->position, prevB->position}, position, stableSeparation);
        nextEdgeA.position.observers.push_back(c2);
        nextEdgeB.position.observers.push_back(c2);
        position.observers.push_back(c2);
        stableSeparation.observers.push_back(c2);*/
    /*}
    ~StructuralEdge() override {
        throw NonRecoverableContradictionException("StructuralEdge destructor called. This should not happen, as StructuralEdges are managed by the system and should not be deleted manually.");
    };*/

    friend struct BiologicalAbomination;
public:

    HalfEdge& getHalfEdge(int index) {
        if (index < 0 || index > 1) {
            throw std::out_of_range("HalfEdge index must be 0 or 1.");
        }
        return halfEdges[index];
    }
    VAxis position[3] = {VAxis(0), VAxis(0), VAxis(0)};
    float stableSeparation;
    Length semanticTension = Length(0);
};

struct BoundaryLoop {
    StructuralSurface& surface;
    HalfEdge& edge;

    [[nodiscard]] int getEdgeCount() const { // TODO temp maybe
        int count = 0;
        HalfEdge* currentEdge = &edge;
        do {
            count++;
            currentEdge = &currentEdge->walk();
        } while (currentEdge != &edge);
        return count;
    }
};

// La cantidad de operadores debe ser el mínimo posible

// Mudando a operadores b-rep Euler
// Operadores topológicos: mitosis, fold
// Operadores semánticos: heredar de otra arista, relaciones, proporciones, angulos, textura
// operadores propagadores: resuelven mediante ecuaciones de las restricciones a traves de las variable y scalar, propagando cambios a otras aristas y superficies

// Invariante: La geometría se renderiza entre cada operación, por lo que los operadores deben recibir geometría renderizable y devolver geometría renderizable.
// TODO Nueva invariante: la geometría es un grafo de grado >=3, cada uno de los vértices de la geometría debe estar conectado a más de 2 aristas, y cada arista debe estar conectada a exactamente 2 superficies. La estructura de datos y los operadores deben reflejarla
struct BiologicalAbomination {
    static void birth(StructuralEdge*& outStructuralEdge, StructuralSurface*& outNewSurfaceA, StructuralSurface*& outNewSurfaceB) {
        outStructuralEdge = new StructuralEdge(*outStructuralEdge);
        outNewSurfaceA = new StructuralSurface();
        outNewSurfaceB = new StructuralSurface();
        outStructuralEdge->halfEdges[0].boundaryLoop = new BoundaryLoop{*outNewSurfaceA, outStructuralEdge->halfEdges[0]};
        outStructuralEdge->halfEdges[1].boundaryLoop = new BoundaryLoop{*outNewSurfaceB, outStructuralEdge->halfEdges[1]};
        outStructuralEdge->halfEdges[0].nextEdge = &outStructuralEdge->halfEdges[0];
        outStructuralEdge->halfEdges[1].nextEdge = &outStructuralEdge->halfEdges[1];

        //  FIXME other properties are UB, do not access before inheriting
    }

    // La posición del nuevo edge siempre será indefinida, activará un TotallyUnrepresentableState.
    // Los structural edges de salida serán promesas que heredarán semántica, no están listos para ser usados.
    static void mitosis(HalfEdge& halfEdgeA, HalfEdge& halfEdgeB, std::vector<StructuralEdge*>& outStructuralEdges, StructuralSurface*& outNewSurface) {
        /*auto newEdge = new StructuralEdge(*surfaces.first, *surfaces.second, *nextEdges.first, *nextEdges.second);
        newEdge->idealSeparation = idealSeparation;
        newEdge->stableSeparation = stableSeparation;
        newEdge->position = position;
        newEdge->position.setPriority(ConstraintSolver::currentFrame);
        newEdge->relation = relation;*/

        auto newEdge = new StructuralEdge(halfEdgeA.structuralEdge); // TODO leak, use pool but idc about leaks rn its the least important thing here
/*
        structuralEdge.halfEdges = {
            HalfEdge(structuralEdge, structuralEdge.halfEdges[0].surface, structuralEdge.halfEdges[0].nextEdge),
            HalfEdge(structuralEdge, newEdge->halfEdges[1].surface, newEdge->halfEdges[1].nextEdge)
        };*/
/*
        // Intercambiar nextEdge correctamente
        for (int i = 0; i < 2; ++i) {
            HalfEdge* originalNext = structuralEdge.halfEdges[i].nextEdge;
            newEdge->halfEdges[i].nextEdge = originalNext;
            structuralEdge.halfEdges[i].nextEdge = &newEdge->halfEdges[i];
        }

        if (newEdge->halfEdges[0].nextEdge == nullptr ||
            newEdge->halfEdges[1].nextEdge == nullptr) {
            throw NonRecoverableContradictionException("Mitosis failed: nextEdge is null.");
            }*/

        // TODO here goes the assert to check if both half edges share vertex

        HalfEdge* originalNext = halfEdgeA.nextEdge;
        halfEdgeA.nextEdge = &newEdge->halfEdges[0];
        newEdge->halfEdges[0].nextEdge = originalNext;
        newEdge->halfEdges[0].boundaryLoop = halfEdgeA.boundaryLoop;

        originalNext = halfEdgeB.nextEdge;
        halfEdgeB.nextEdge = &newEdge->halfEdges[1];
        newEdge->halfEdges[1].nextEdge = originalNext;
        newEdge->halfEdges[1].boundaryLoop = halfEdgeB.boundaryLoop;

        outStructuralEdges.push_back(newEdge);

        // TODO if surfaces of both half edges are adjacent create an extra edge and extra surface to avoid < 3 degree node.

        auto newEdge2 = new StructuralEdge(halfEdgeB.structuralEdge);
        outStructuralEdges.push_back(newEdge2);

        outNewSurface = new StructuralSurface();

        originalNext = halfEdgeA.nextEdge;
        halfEdgeA.nextEdge = &newEdge2->halfEdges[0];
        newEdge2->halfEdges[0].nextEdge = originalNext->nextEdge->nextEdge;
        originalNext->nextEdge->nextEdge = &newEdge2->halfEdges[1];
        newEdge2->halfEdges[1].nextEdge = originalNext;
        newEdge2->halfEdges[0].boundaryLoop = halfEdgeA.boundaryLoop;
        newEdge2->halfEdges[1].boundaryLoop = new BoundaryLoop{*outNewSurface, newEdge2->halfEdges[1]};


    }
/*  Merging this code with mitosis.
    static void fold(HalfEdge& edgeA, HalfEdge& edgeB, StructuralEdge*& outStructuralEdge, StructuralSurface*& outStructuralSurface) {
        StructuralSurface* newSurface = new StructuralSurface();
        StructuralEdge* newEdge = new StructuralEdge(edgeA.structuralEdge); // TODO complete

        if (&edgeA.surface != &edgeB.surface) {
            throw NonRecoverableContradictionException("Cannot fold edges from different surfaces.");
        }

        HalfEdge* h = &edgeA;

        while (h != &edgeB) {
            h->surface = newSurface;
            h = h->nextEdge;
        }
        edgeB.surface = newSurface;

        outStructuralEdge = newEdge;
        outStructuralSurface = newSurface;
    }*/
/*
    template <class T>
    static void inherit(Inherited<T>& fromEdge, StructuralEdge& toEdge) { // rethink, nascent idea
        toEdge.surfaceTypes = fromEdge.surfaceTypes;
        toEdge.edgeLength = fromEdge.edgeLength;
        toEdge.lengthOverload = fromEdge.lengthOverload;
        toEdge.proportionalRelation = fromEdge.proportionalRelation;
        toEdge.semanticSeparation = fromEdge.semanticSeparation;
        toEdge.nearestSemantic = fromEdge.nearestSemantic;

        fromEdge is AngularSeparationInherited
    }*/

    static void reWeight() { // rethink, nascent idea
        ConstraintSolver::checkAndSolve();
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
