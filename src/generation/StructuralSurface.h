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

    void collapseDisplayList() {
        if (edges.size() < 3) {
            //throw NonRecoverableContradictionException("Cannot collapse display list with less than 3 edges.");
        }

        static Vtx vertexBuffer[64]; // ajustar tamaño al máximo de edges esperado

        std::visit(overload{
            [&](const RepeatUV& t) {
                /*
                for (size_t i = 0; i < edges.size(); i++) {
                    Vector3 pos = edges[i]->getPosition();
                    vertexBuffer[i] = {{
                        {(s16) pos.x, (s16) pos.y, (s16) pos.z},
                        0,
                        {(s16) 0, (s16) 0}, // TODO: uv real usando t.tileWidth/t.tileHeight
                        {(u8) (normal.x * 127), (u8) (normal.y * 127), (u8) (normal.z * 127), 255}
                    }};
                }*/
            },
            [&](const StretchUV& e) {
                /*
                for (size_t i = 0; i < edges.size(); i++) {
                    Vector3 pos = edges[i]->getPosition();
                    vertexBuffer[i] = {{
                        {(s16) pos.x, (s16) pos.y, (s16) pos.z},
                        0,
                        {(s16) 0, (s16) 0}, // TODO: uv real usando e.startPixel/e.endPixel
                        {(u8) (normal.x * 127), (u8) (normal.y * 127), (u8) (normal.z * 127), 255}
                    }};
                }*/
            }
        }, uvConfig.mode);

        // 2) Inyectar realmente los comandos en gGfxPool a través de gDisplayListHead.
        //    gSPVertex carga los vértices al RSP; gSP1Triangle arma cada triángulo.
        //    Cada llamada escribe en *gDisplayListHead y lo incrementa (++).

        DisplayVertex vertices[3] = {
            DisplayVertex{Vector3{-1000, 0, 1000}, 0, 0, 32, Vector3{0, 1, 0}},
            DisplayVertex{Vector3{1000, 0, 1000}, 0, 0, 32, Vector3{0, 1, 0}},
            DisplayVertex{Vector3{0, 0, -1000}, 0, 0, 32, Vector3{0, 1, 0}}
        }; // Placeholder
        DisplayListManager::addDisplayListSegment(std::vector<DisplayVertex>(vertices, vertices + 3), std::vector<std::array<int, 3>>{{0, 1, 2}}, inside_0900A000);
    }

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