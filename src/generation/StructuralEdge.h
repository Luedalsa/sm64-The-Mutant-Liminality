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
    friend struct StructuralEdge;
private:
    StructuralSurface* surface;
    StructuralEdge* nextEdge;
    HalfEdge* twin;

    friend class BiologicalAbomination;
public:
    //Variable<bool> semanticallyPrecise = Variable(false, 0);
    TriangleFaceType stableSurfaceType;

    void changeSurface(StructuralSurface& newSurface) {
        surface = &newSurface;
    }

    void changeNextEdge(StructuralEdge& edge) { nextEdge = &edge; }
};

// Nótese que StructuralEdge no tiene un constructor por defecto, ya que siempre debe estar asociado a dos superficies y dos edges siguientes. Esto garantiza que la estructura de la geometría sea consistente y evita la creación de edges huérfanos o mal definidos.
// Es una invariante del sistema que cada StructuralEdge esté correctamente conectado a sus superficies y edges adyacentes, tambien reduce las vulnerabilidades en la estructura de datos en runtime, no necesita asserts y reduce la carga cognitiva de mantener toda la arquitectura en mi memoria de trabajo al momento de establecer restricciones
struct StructuralEdge : BaseEdge, Diagnosable {
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

    friend class BiologicalAbomination;
public:

    VAxis position[3] = {VAxis(0), VAxis(0), VAxis(0)};
    float idealSeparation = 3.14159265358979323846f/2.0f; // 90 degrees in radians
    Theta stableSeparation = Theta(3.14159265358979323846f/2.0f); // 90 degrees in radians
    Length semanticTension = Length(0);
};


struct BiologicalAbomination {
    static void mitosis(StructuralEdge& parent, const SemanticEdge &edge, HalfEdge& outHalfEdgeA, HalfEdge& outHalfEdgeB, StructuralEdge& outStructuralEdge) {
        /*auto newEdge = new StructuralEdge(*surfaces.first, *surfaces.second, *nextEdges.first, *nextEdges.second);
        newEdge->idealSeparation = idealSeparation;
        newEdge->stableSeparation = stableSeparation;
        newEdge->position = position;
        newEdge->position.setPriority(ConstraintSolver::currentFrame);
        newEdge->relation = relation;*/

        outHalfEdgeA = *(new HalfEdge());
        outHalfEdgeB = *(new HalfEdge());
        outStructuralEdge = *(new StructuralEdge());
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
