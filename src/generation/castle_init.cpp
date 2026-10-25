#include "ConstraintOperator.h"
#include "behavior_data.h"

#include <utility>
#include "level_commands.h"
#include "engine/surface_collision.h"
#include "DebugExport.h"
#include "LevelScriptManager.h"
#include "SpaceAnchor.h"
#include "StructuralFace.h"
#include "game/ingame_menu.h"

inline int CASTLE_SEED = 0xDEADBEEF;

// SM64: The Mutant Liminality. Procedural Geometry Generation.
// I think this is a good name

extern "C" {
#include <ultra64.h>
#include "castle_init.h"
#include <geo_commands.h>

#include "game/save_file.h"
#include "types.h"

} // extern "C"


s32 castle_init(s16 arg, s32 unused);

constexpr LevelScript emptyScript = {
    RETURN(),
};
LevelScript* mutantCastleLevelScript = (LevelScript*)&emptyScript;

// Ensure C linkage for the level script symbol so C code can reference it
extern "C" LevelScript level_castle_inside_entry[] = {
    CALL(/*arg*/ 0, /*func*/ castle_init ),
    JUMP_LINK(mutantCastleLevelScript),
    EXIT(),
};
extern "C" u8 dialog_text_DIALOG_133[5000] = { DIALOG_CHAR_TERMINATOR };

// Las estructuras de datos están diseñadas para evitar vulnerabilidades en las estructuras de datos durante la generación de geometría y solución de restricciones. evitando infernales sesiones de debug.
// Algunas claves de mi metodología:
// Todo es referencia hasta que se demuestre la necesidad de un puntero nulo
// Todo es privado hasta que se demuestre la necesidad de una interfaz
// Todo es constante hasta que se demuestre la necesidad de una variable
// Todo es singleton hasta que se demuestre la necesidad de instancias múltiples
s32 castle_init(s16 arg, s32 unused) {
    CASTLE_SEED = 0;

    save_file_set_flags(SAVE_FLAG_HAVE_WING_CAP | SAVE_FLAG_HAVE_METAL_CAP | SAVE_FLAG_HAVE_VANISH_CAP);

    std::string s = "Hello, Mario... \n The castle keeps \n changing? What are you \n talking about? it \n always looked like this \n [GENERATD BY SEED" + std::to_string(CASTLE_SEED) + "]\n";

    for (int i = 0; i < s.length() && i < 5000; ++i) {
        dialog_text_DIALOG_133[i] = ASCII_TO_DIALOG(s[i]);
        if (s[i] == '\n') {
            dialog_text_DIALOG_133[i] = DIALOG_CHAR_NEWLINE;
        } else if (s[i] == ' ') {
            dialog_text_DIALOG_133[i] = DIALOG_CHAR_SPACE;
        } else if (s[i] == '.') {
            dialog_text_DIALOG_133[i] = DIALOG_CHAR_PERIOD;
        } else if (s[i] == ',') {
            dialog_text_DIALOG_133[i] = DIALOG_CHAR_COMMA;
        }
    }
    dialog_text_DIALOG_133[s.length()] = DIALOG_CHAR_TERMINATOR;

    LevelScriptManager::setup();


    auto* e = static_cast<StructuralEdge *>(malloc(sizeof(StructuralEdge)));
    std::vector<StructuralEdge*> e2;
    StructuralFace* newSurfaceA;

    ConstraintOperator::abiogenesis(e);
    ConstraintOperator::cellWallGrow(e->getHalfEdge(1), e->getHalfEdge(0).walk().walk(), e2, newSurfaceA);
    newSurfaceA = &e->getHalfEdge(0).getBoundaryLoop().getSurface();

    SpaceAnchor::pivotHalfEdge = &e->getHalfEdge(0);
    SpaceAnchor::pivotTransform = RelativeTransform{1000.0f, 3.141592f/2.0f, 0.0f};
/*
    pivotSurface->normal.set(Vector3(0, 1, 0));
    pivotSurface->normal.setPriority(ConstraintSolver::currentFrame);
    pivotEdge->position.set(Vector3(1000, 0, 0));
    pivotEdge->position.setPriority(ConstraintSolver::currentFrame);
        nextEdges.first->position = Vector3(pos.x + transform.distance * cos(transform.yaw),
                                            pos.y + transform.height,
                                            pos.z + transform.distance * sin(transform.yaw));
        nextEdges.first->position.priority = ConstraintSolver::currentFrame;
    StructuralEdge* next = (pivotEdge->getSurfaces().first == pivotSurface ? pivotEdge->getNextEdges().first : pivotEdge->getNextEdges().second);
    next->setPosition(Vector3(1000, 0, 1000));*/

    // Manual position debug test TODO: DELETE
    e->displayVertex.position.x = 1000.0f;
    e->displayVertex.position.y = 0.0f;
    e->displayVertex.position.z = 1000.0f;
    e = &e->getHalfEdge(0).walk().structuralEdge;
    e->displayVertex.position.x = 1000.0f;
    e->displayVertex.position.y = 0.0f;
    e->displayVertex.position.z = -1000.0f;
    e = &e->getHalfEdge(0).walk().structuralEdge;
    e->displayVertex.position.x = -1000.0f;
    e->displayVertex.position.y = 0.0f;
    e->displayVertex.position.z = -1000.0f;
    e = &e->getHalfEdge(0).walk().structuralEdge;
    e->displayVertex.position.x = -1000.0f;
    e->displayVertex.position.y = 0.0f;
    e->displayVertex.position.z = 1000.0f;

    newSurfaceA->collapseDisplayList();
    newSurfaceA->collapseTerrain();

    // exportTrianglesToObj("debug_pass1.obj", "ok");

    mutantCastleLevelScript = LevelScriptManager::buildLevel();
    level_castle_inside_entry[3] = (LevelScript)mutantCastleLevelScript;

    //MutantCastle mutantCastle = MutantCastle();
    // mutantCastle.build();

    return 0;
}