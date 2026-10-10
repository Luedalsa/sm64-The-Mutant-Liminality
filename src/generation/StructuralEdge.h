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

struct HalfEdge {
    StructuralSurface* surface;
    StructuralEdge* nextEdge;
    //Variable<bool> semanticallyPrecise = Variable(false, 0);
    TriangleFaceType stableSurfaceType;

    void changeSurface(StructuralSurface& newSurface) {
        surface = &newSurface;
    }
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
public:
    StructuralEdge(StructuralSurface& surfaceA, StructuralSurface& surfaceB, StructuralEdge& nextEdgeA, StructuralEdge& nextEdgeB) /*: surfaces(&surfaceA, &surfaceB), nextEdges(&nextEdgeA, &nextEdgeB)*/ {
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
    }
    ~StructuralEdge() override {
        throw NonRecoverableContradictionException("StructuralEdge destructor called. This should not happen, as StructuralEdges are managed by the system and should not be deleted manually.");
    };

    std::pair<HalfEdge, HalfEdge> halfEdges;
    VAxis x = VAxis(0);
    VAxis y = VAxis(0);
    VAxis z = VAxis(0);
    //VVector3 position = VVector3(x, y, z, INT32_MAX);
    float idealSeparation = 3.14159265358979323846f/2.0f; // 90 degrees in radians
    Theta stableSeparation = Theta(3.14159265358979323846f/2.0f); // 90 degrees in radians
    Length semanticTension = Length(0);
    /*std::pair<StructuralSurface*, StructuralSurface*> getSurfaces() const { return surfaces; }
    void changeNextEdgeA(StructuralEdge& edge) { nextEdges.first = &edge; }
    void changeNextEdgeB(StructuralEdge& edge) { nextEdges.second = &edge; }
    std::pair<StructuralEdge*, StructuralEdge*> getNextEdges() const { return nextEdges; }*/
    void mitosis(StructuralEdge *parent, const SemanticEdge &edge) {
        /*auto newEdge = new StructuralEdge(*surfaces.first, *surfaces.second, *nextEdges.first, *nextEdges.second);
        newEdge->idealSeparation = idealSeparation;
        newEdge->stableSeparation = stableSeparation;
        newEdge->position = position;
        newEdge->position.setPriority(ConstraintSolver::currentFrame);
        newEdge->relation = relation;*/
    }
/*
    void diagnose() override {
        if (surfaces.first == nullptr || surfaces.second == nullptr) {
            addDiagnostic(Diagnostic::NOT_SATISFIED_EDGE_SURFACES);
        } else {
            if (surfaces.first->faceType != idealFaceTypes.first || surfaces.second->faceType != idealFaceTypes.second) {
                addDiagnostic(Diagnostic::SEMANTIC_TENSION);
            }*//*
            for (const Diagnostic* diagnostic : {surfaces.first->getDiagnostics().data(), surfaces.second->getDiagnostics().data()}) {
                if (diagnostic == Diagnostic::NOT_SATISFIED_SURFACES_EDGES) {
                    return; // If either surface has unsatisfied edges, we cannot check angle violation
                }
            }*//*
            Vector3 normal1 = surfaces.first->getNormal();
            Vector3 normal2 = surfaces.second->getNormal();*//*
            float angle = std::acos(normal1.dot(normal2) / (normal1.length() * normal2.length()));
            if (std::abs(angle - idealSeparation) > 0.1f) { // Allow a small tolerance
                addDiagnostic(Diagnostic::ANGLE_VIOLATION);
            }
        }
    }*/
};

#endif // SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
