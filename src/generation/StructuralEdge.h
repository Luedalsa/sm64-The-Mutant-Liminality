//
// Created by Luis Alvarez on 16/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
#define SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
#include "BaseEdge.h"
#include "Constraintsolver.h"
#include "Diagnosable.h"
#include "SemanticEdge.h"
#include "StructuralSurface.h"

#include <vector>

class StructuralSurface;
struct BiologicalAbomination;

class HalfEdge {
private:
    StructuralSurface* surface;
    HalfEdge* nextEdge;

    friend struct BiologicalAbomination;
public:
    //Variable<bool> semanticallyPrecise = Variable(false, 0);
    TriangleFaceType stableSurfaceType;
};

// Nótese que StructuralEdge no tiene un constructor por defecto, ya que siempre debe estar asociado a dos superficies y dos edges siguientes. Esto garantiza que la estructura de la geometría sea consistente y evita la creación de edges huérfanos o mal definidos.
// Es una invariante del sistema que cada StructuralEdge esté correctamente conectado a sus superficies y edges adyacentes, tambien reduce las vulnerabilidades en la estructura de datos en runtime, no necesita asserts y reduce la carga cognitiva de mantener toda la arquitectura en mi memoria de trabajo al momento de establecer restricciones
struct Edge : BaseEdge, Diagnosable {
private:
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

    VAxis position[3] = {VAxis(0), VAxis(0), VAxis(0)};
    float idealSeparation = 3.14159265358979323846f/2.0f; // 90 degrees in radians
    Theta stableSeparation = Theta(3.14159265358979323846f/2.0f); // 90 degrees in radians
    Length semanticTension = Length(0);
};

struct StructuralEdge {
private:
    StructuralEdge() {
        halfEdges[0] = HalfEdge();
        halfEdges[1] = HalfEdge();
        edge = Edge();
    }
    friend struct BiologicalAbomination;
public:
    HalfEdge halfEdges[2];
    Edge edge;
};


struct BiologicalAbomination {
    static void mitosis(StructuralEdge structuralEdge, StructuralEdge*& outStructuralEdge) {
        /*auto newEdge = new StructuralEdge(*surfaces.first, *surfaces.second, *nextEdges.first, *nextEdges.second);
        newEdge->idealSeparation = idealSeparation;
        newEdge->stableSeparation = stableSeparation;
        newEdge->position = position;
        newEdge->position.setPriority(ConstraintSolver::currentFrame);
        newEdge->relation = relation;*/

        auto newEdge = new StructuralEdge();

        newEdge->halfEdges[0].surface = structuralEdge.halfEdges[0].surface;
        newEdge->halfEdges[0].nextEdge = structuralEdge.halfEdges[0].nextEdge;

        structuralEdge.halfEdges[0].nextEdge = &newEdge->halfEdges[0];
        newEdge->halfEdges[1].surface = structuralEdge.halfEdges[1].surface;
        newEdge->halfEdges[1].nextEdge = structuralEdge.halfEdges[1].nextEdge;
        structuralEdge.halfEdges[1].nextEdge = &newEdge->halfEdges[1];

        outStructuralEdge = newEdge;
    }

    static void fold(StructuralEdge& edgeA, StructuralEdge& edgeB, StructuralEdge*& outStructuralEdge, StructuralSurface*& outStructuralSurface) {
        StructuralEdge* newEdge = new StructuralEdge();

        StructuralSurface* newSurface = new StructuralSurface();

        // find common half edge
        if (edgeA.halfEdges[0].surface == edgeB.halfEdges[0].surface) {
            newEdge->halfEdges[0] = edgeA.halfEdges[0];
            newEdge->halfEdges[1] = edgeB.halfEdges[0];
            newSurface = edgeA.halfEdges[0].surface;
        } else if (edgeA.halfEdges[0].surface == edgeB.halfEdges[1].surface) {
            newEdge->halfEdges[0] = edgeA.halfEdges[0];
            newEdge->halfEdges[1] = edgeB.halfEdges[1];
            newSurface = edgeA.halfEdges[0].surface;
        } else if (edgeA.halfEdges[1].surface == edgeB.halfEdges[0].surface) {
            newEdge->halfEdges[0] = edgeA.halfEdges[1];
            newEdge->halfEdges[1] = edgeB.halfEdges[0];
            newSurface = edgeA.halfEdges[1].surface;
        } else if (edgeA.halfEdges[1].surface == edgeB.halfEdges[1].surface) {
            newEdge->halfEdges[0] = edgeA.halfEdges[1];
            newEdge->halfEdges[1] = edgeB.halfEdges[1];
            newSurface = edgeA.halfEdges[1].surface;
        } else {
            throw NonRecoverableContradictionException("Cannot fold edges: no common surface found.");
        }

        // TODO complete

        outStructuralEdge = newEdge;
        outStructuralSurface = newSurface;
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
