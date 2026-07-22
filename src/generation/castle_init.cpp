#include "behavior_data.h"
#include "../game/debug_course.h"

#include <cstring>
#include <utility>
#include <vector>
#include "level_commands.h"
#include "engine/surface_collision.h"
#include "levels/castle_inside/header.h"
#include "AgentManager.h"
#include "DebugExport.h"
#include "LevelScriptManager.h"
#include "game/ingame_menu.h"

inline int CASTLE_SEED = 0xDEADBEEF;

// SM64: The Mutant Liminality. Procedural Geometry Generation.
// I think this is a good name

extern "C" {
#include <ultra64.h>
#include "castle_init.h"
#include <level_misc_macros.h>
#include <special_preset_names.h>
#include <textures.h>
#include <geo_commands.h>

#include "game/level_geo.h"
#include "game/geo_misc.h"
#include "types.h"

#include "sm64.h"
#include "../game/camera.h"

#include "model_ids.h"
#include "seq_ids.h"
#include "dialog_ids.h"

#include "game/level_update.h"

#include "levels/scripts.h"

#include "actors/common1.h"

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

s32 castle_init(s16 arg, s32 unused) {
    CASTLE_SEED = 0;

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

    AgentManager::setup();
    LevelScriptManager::setup();

    AgentManager::start();

    exportTrianglesToObj("debug_pass1.obj", "ok");

    mutantCastleLevelScript = LevelScriptManager::buildLevel();
    level_castle_inside_entry[3] = (LevelScript)mutantCastleLevelScript;

    //MutantCastle mutantCastle = MutantCastle();
    //mutantCastle.build();

    return 0;
}