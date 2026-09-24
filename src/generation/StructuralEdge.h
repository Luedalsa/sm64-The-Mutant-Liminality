//
// Created by Luis Alvarez on 16/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
#define SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
#include "BaseEdge.h"
#include "Diagnosable.h"
#include "SemanticEdge.h"
#include "StructuralSurface.h"

#include <vector>

class StructuralSurface;

struct StructuralEdge : BaseEdge, Diagnosable {
    std::pair<StructuralSurface *, StructuralSurface *> surfaces;
    float idealSeparation = 3.14159265358979323846f/2.0f; // 90 degrees in radians
public:
    void mitosis(StructuralEdge *parent, const SemanticEdge &edge) {
    }

    void diagnose() override {
        if (surfaces.first == nullptr || surfaces.second == nullptr) {
            addDiagnostic(Diagnostic::NOT_SATISFIED_EDGE_SURFACES);
        } else {/*
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
            }*/
        }
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
