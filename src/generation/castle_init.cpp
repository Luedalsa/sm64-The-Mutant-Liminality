#include "behavior_data.h"
#include "../game/debug_course.h"

#include <cstring>
#include <utility>
#include <vector>
#include "level_commands.h"
#include "engine/surface_collision.h"
#include "levels/castle_inside/header.h"
#include "AgentManager.h"
#include "LevelScriptManager.h"

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

s32 castle_init(s16 arg, s32 unused) {
    CASTLE_SEED = 0;

    AgentManager::setup();
    LevelScriptManager::setup();

    AgentManager::start();
    mutantCastleLevelScript = LevelScriptManager::buildLevel();
    level_castle_inside_entry[3] = (LevelScript)mutantCastleLevelScript;

    //MutantCastle mutantCastle = MutantCastle();
    //mutantCastle.build();

    return 0;
}