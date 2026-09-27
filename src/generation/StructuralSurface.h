//
// Created by Luis Alvarez on 19/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_STRUCTURALSURFACE_H
#define SM64_THE_MUTANT_LIMINALITY_STRUCTURALSURFACE_H

#include "Diagnosable.h"
#include "DisplayListManager.h"
#include "NonRecoverableContradictionException.h"
#include "Primitives.h"
#include "TriangleTypes.h"
#include "UVModes.h"
#include "game/game_init.h"

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

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

struct Triangulation {
    struct Leaf { StructuralEdge* boundaryEdge; };
    struct Fork {
        std::unique_ptr<Triangulation> left, right;
    };
    std::variant<Leaf, Fork> node;
};

class StructuralSurface : public Diagnosable {
private:
    Vector3 normal;
public:
    std::vector<StructuralEdge*> edges;
    std::unique_ptr<Triangulation> triangulation;
    TriangleFaceType faceType = TriangleFaceType::None;
    SurfaceUVConfig uvConfig = SurfaceUVConfig{StretchUV{0, 32}};

    Vector3 getNormal() const {
        if (edges.size() < 3) {
            throw NonRecoverableContradictionException("Cannot compute surface normal with less than 3 edges.");
        }
        return normal;
    }

    void diagnose() override {
        if (edges.size() < 3) {
            addDiagnostic(Diagnostic::NOT_SATISFIED_SURFACES_EDGES);
        }
    }

    void collapseDisplayList();

    void collapseTerrain() {
        if (edges.size() < 3) {
            throw NonRecoverableContradictionException("Cannot collapse terrain with less than 3 edges.");
        }
        for (auto edge : edges) {
        }
    }
};

#endif

#endif // SM64_THE_MUTANT_LIMINALITY_STRUCTURALSURFACE_H