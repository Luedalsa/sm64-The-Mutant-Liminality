//
// Created by Luis Alvarez on 19/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_STRUCTURALFACE_H
#define SM64_THE_MUTANT_LIMINALITY_STRUCTURALFACE_H

#include "Dependent.h"
#include "Diagnosable.h"
#include "StructuralEdge.h"
#include "TriangleTypes.h"
#include "UVModes.h"

#ifdef __cplusplus
extern "C" {
#endif

    #include "game/game_init.h" // Gfx, Vtx, gDisplayListHead, y macros gSP*/gDP* de PR/gbi.h
    // gGfxPool y gDisplayListHead ya quedan declarados al incluir game_init.h arriba.

#ifdef __cplusplus
}

#include <memory>
#include <variant>
#include <vector>

struct StructuralEdge;
class ARN;

struct Triangulation {
    struct Leaf { StructuralEdge* boundaryEdge; };
    struct Fork {
        std::unique_ptr<Triangulation> left, right;
    };
    std::variant<Leaf, Fork> node;
};

class StructuralFace : public Diagnosable {
public:
    Theta normal[2] = {Theta(0.0f), Theta(0.0f)};
    StructuralFace();
    ~StructuralFace();
    ;

    std::unique_ptr<Triangulation> triangulation;
    TriangleFaceType faceType = TriangleFaceType::None;
    SurfaceUVConfig uvConfig = SurfaceUVConfig{StretchUV{0, 32}};

    std::vector<ARN> mitosisPlans;
/*
    void diagnose() override {
        if (edges.size() < 3) {
            addDiagnostic(Diagnostic::NOT_SATISFIED_SURFACES_EDGES);
        }
    }
*/
    void collapseDisplayList();

    void collapseTerrain();
};

#endif

#endif // SM64_THE_MUTANT_LIMINALITY_STRUCTURALFACE_H