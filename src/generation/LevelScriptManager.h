//
// Created by Luis Alvarez on 30/06/2026.
//

#ifndef SM64_PORT_LEVELSCRIPTMANAGER_H
#define SM64_PORT_LEVELSCRIPTMANAGER_H

#include "ActorSpawnerManager.h"
#include "CollisionManager.h"
#include "DisplayListManager.h"
#include "LevelPoolManager.h"
#include "VertexCollection.h"
#include "TriangleCollection.h"
#include "textures.h"
#include "actors/group15.h"

#include <cstdlib>
#include <cstring>

#include <vector>
#include <queue>
#include <stdexcept>

// SM64: The Mutant Liminality. Procedural Geometry Generation.
// I think this is a good name

extern "C" {
#include <ultra64.h>

#include "types.h"

#include "model_ids.h"
#include "seq_ids.h"
#include "dialog_ids.h"

#include "game/level_update.h"

#include "levels/scripts.h"

#include "actors/common1.h"

#include "behavior_data.h"
#include "../game/camera.h"
#include "geo_commands.h"
#include "../game/geo_misc.h"

#include "level_commands.h"
#include "../game/level_geo.h"
#include "level_misc_macros.h"
#include "sm64.h"
#include "special_preset_names.h"
#include "levels/castle_inside/header.h"
#include "surface_terrains.h"
#include "actors/common0.h"
#include "actors/group10.h"
#include "actors/group9.h"

} // extern "C"

class LevelScriptManager {
    static std::queue<int> trianglesQueue;

    template <typename T, typename... Args>
    static void writeMacro(T*& p, Args... args) {
        ((*p++ = static_cast<T>(args)), ...); // fold expression, C++17
    }

    static GeoLayout* buildGeoBranch() {
        const GeoLayout geoBranch[] = {
            GEO_NODE_START(),
            GEO_OPEN_NODE(),
               GEO_DISPLAY_LIST(LAYER_OPAQUE, DisplayListManager::buildDisplayList()),
               GEO_ASM(0, geo_exec_inside_castle_light),
            GEO_CLOSE_NODE(),
            GEO_RETURN(),
        };

        auto newGeoBranch = LevelPoolManager::allocOnPool<GeoLayout>(sizeof(geoBranch) / sizeof(GeoLayout));
        std::memcpy(newGeoBranch, geoBranch, sizeof(geoBranch));

        return newGeoBranch;
    }

    static GeoLayout* buildGeoLayout() {
        const GeoLayout geoLayout[] = {
           GEO_NODE_SCREEN_AREA(10, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH/2, SCREEN_HEIGHT/2),
           GEO_OPEN_NODE(),
              GEO_ZBUFFER(0),
              GEO_OPEN_NODE(),
                 GEO_NODE_ORTHO(100),
                 GEO_OPEN_NODE(),
                    GEO_BACKGROUND_COLOR(0x0001),
                 GEO_CLOSE_NODE(),
              GEO_CLOSE_NODE(),
              GEO_ZBUFFER(1),
              GEO_OPEN_NODE(),
                 GEO_CAMERA_FRUSTUM_WITH_FUNC(64, 50, 7000, geo_camera_fov),
                 GEO_OPEN_NODE(),
                    GEO_CAMERA(13, 0, 2000, 6000, 0, 0, 0, geo_camera_main),
                    GEO_OPEN_NODE(),
                       GEO_SWITCH_CASE(1, geo_switch_area),
                       GEO_OPEN_NODE(),
                          GEO_BRANCH(1, buildGeoBranch()),
                       GEO_CLOSE_NODE(),
                       GEO_RENDER_OBJ(),
                       GEO_ASM(0, geo_envfx_main),
                    GEO_CLOSE_NODE(),
                 GEO_CLOSE_NODE(),
              GEO_CLOSE_NODE(),
           GEO_CLOSE_NODE(),
           GEO_END(),
        };

        auto newGeoLayout = LevelPoolManager::allocOnPool<GeoLayout>(sizeof(geoLayout) / sizeof(GeoLayout));
        std::memcpy(newGeoLayout, geoLayout, sizeof(geoLayout));

        return newGeoLayout;
    }

    static Collision* buildTerrain() {
        //return (short*)&inside_castle_seg7_area_1_collision[0]; // Default collision for reference

        auto collisionVertices = CollisionManager::getCollisionVertices();
        auto collisionTriangles = CollisionManager::getCollisionTriangles();

        // Security floor
        /*std::vector<Vector3> collisionVertices{{16000, 0, 16000}, {16000, 0, -16000}, {-16000, 0, 16000}, {-16000, 0, -16000}};
        std::unordered_map<int, std::vector<int>> collisionTriangles{{SURFACE_DEFAULT, {0, 1, 2, 2, 1, 3}}};*/

        auto triangles = 0;
        for (auto& t : collisionTriangles) {
            triangles += t.second.size();
        }

        auto newTerrain = LevelPoolManager::allocOnPool<Collision>(4 + collisionVertices.size() * 2 + triangles * 2 + collisionTriangles.size() * 2 + 30); // TODO do proper calculations

        auto p = newTerrain;
        writeMacro(p, COL_INIT());
        writeMacro(p, COL_VERTEX_INIT(static_cast<short int>(collisionVertices.size())));

        for (auto v : collisionVertices) {
            *p = v.x;
            p++;
            *p = v.y;
            p++;
            *p = v.z;
            p++;
        }

        for (auto& t : collisionTriangles) {
            if (!t.second.empty()) {
                *p = t.first;
                p++;
                *p = t.second.size()/3;
                p++;
                for (auto t : t.second) {
                    *p = t;
                    p++;
                }
            }
        }

        writeMacro(p, COL_TRI_STOP());
        auto doors = ActorSpawnerManager::getSpecialDoors();
        writeMacro(p, COL_SPECIAL_INIT(8 + doors.size()));
        for (auto& d : doors) {
            writeMacro(p, SPECIAL_OBJECT_WITH_YAW(/*preset*/ special_wooden_door, /*pos*/ d[0], d[1], d[2], /*yaw*/ d[3]));
        }
        writeMacro(p, SPECIAL_OBJECT_WITH_YAW_AND_PARAM(/*preset*/ special_castle_door_warp,  /*pos*/ -1100,    0,  2202, /*yaw*/   0, /*behParam2*/ 0));
        writeMacro(p, SPECIAL_OBJECT_WITH_YAW_AND_PARAM(/*preset*/ special_castle_door_warp,  /*pos*/ -946,     0,  2202, /*yaw*/ 128, /*behParam2*/ 1));
        writeMacro(p, SPECIAL_OBJECT_WITH_YAW_AND_PARAM(/*preset*/ special_wooden_door_warp,  /*pos*/ -1023, -101, -5170, /*yaw*/   0, /*behParam2*/ 2));
        writeMacro(p, SPECIAL_OBJECT_WITH_YAW(/*preset*/ special_0stars_door,       /*pos*/ -3122,  205,  -793, /*yaw*/  64));
        writeMacro(p, SPECIAL_OBJECT_WITH_YAW(/*preset*/ special_1star_door,        /*pos*/   256,    0, -1074, /*yaw*/   0));
        writeMacro(p, SPECIAL_OBJECT_WITH_YAW(/*preset*/ special_1star_door,        /*pos*/   644,  614, -1476, /*yaw*/ 224));
        writeMacro(p, SPECIAL_OBJECT_WITH_YAW(/*preset*/ special_3star_door,        /*pos*/  1075,  205,  -229, /*yaw*/ 192));
        writeMacro(p, SPECIAL_OBJECT_WITH_YAW(/*preset*/ special_3star_door,        /*pos*/ -2303,    0, -1074, /*yaw*/   0));
        writeMacro(p, COL_END());

        return newTerrain;
    }

    static char* buildRooms() {
        auto newRooms = LevelPoolManager::allocOnPool<char>(1);
        newRooms[0] = 1;

        return newRooms;
    }

    static LevelScript* buildObjects() {
        auto goombas = ActorSpawnerManager::getGoombas();
        auto newObjects = LevelPoolManager::allocOnPool<LevelScript>(6 + goombas.size() * 6 + 1);

        auto p = newObjects;

        writeMacro(p, OBJECT(/*model*/ MODEL_TOAD,       /*pos*/ -1671,    0,  1313, /*angle*/ 0,  83, 0, /*behParam*/ DIALOG_133 << 24, /*beh*/ bhvToadMessage));
        for (auto& o : goombas) {
            writeMacro(p, OBJECT(/*model*/ MODEL_GOOMBA,           /*pos*/  o[0], o[1],  o[2], /*angle*/ 0,   0, 0, /*behParam*/ 0x00010000, /*beh*/ bhvGoomba));
        }
        writeMacro(p, RETURN());
        return newObjects;
    }

    static LevelScript* buildLevelScript() {
        LevelScript scriptData[] = {
            INIT_LEVEL(),
            //LOAD_MIO0(        /*seg*/ 0x07, _castle_inside_segment_7SegmentRomStart, _castle_inside_segment_7SegmentRomEnd),
            //LOAD_MIO0_TEXTURE(/*seg*/ 0x09, _inside_mio0SegmentRomStart, _inside_mio0SegmentRomEnd),
            //LOAD_MIO0(        /*seg*/ 0x06, _group15_mio0SegmentRomStart, _group15_mio0SegmentRomEnd),
            //LOAD_RAW(         /*seg*/ 0x0D, _group15_geoSegmentRomStart,  _group15_geoSegmentRomEnd),
            ALLOC_LEVEL_POOL(),
            MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
            LOAD_MODEL_FROM_GEO(MODEL_MIPS,                      mips_geo),
            LOAD_MODEL_FROM_GEO(MODEL_BOO_CASTLE,                boo_castle_geo),
            LOAD_MODEL_FROM_GEO(MODEL_LAKITU,                    lakitu_geo),
            LOAD_MODEL_FROM_GEO(MODEL_TOAD,                      toad_geo),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_BOWSER_TRAP,        castle_geo_000F18),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_WATER_LEVEL_PILLAR, castle_geo_001940),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_CLOCK_MINUTE_HAND,  castle_geo_001530),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_CLOCK_HOUR_HAND,    castle_geo_001548),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_CLOCK_PENDULUM,     castle_geo_001518),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_CASTLE_DOOR,        castle_door_geo),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_WOODEN_DOOR,        wooden_door_geo),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_METAL_DOOR,         metal_door_geo),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_CASTLE_DOOR_UNUSED, castle_door_geo),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_WOODEN_DOOR_UNUSED, wooden_door_geo),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_DOOR_0_STARS,       castle_door_0_star_geo),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_DOOR_1_STAR,        castle_door_1_star_geo),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_DOOR_3_STARS,       castle_door_3_stars_geo),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_KEY_DOOR,           peach_geo_000098),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_STAR_DOOR_30_STARS, castle_geo_000F00),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_STAR_DOOR_8_STARS,  castle_geo_000F00),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_STAR_DOOR_50_STARS, castle_geo_000F00),
            LOAD_MODEL_FROM_GEO(MODEL_CASTLE_STAR_DOOR_70_STARS, castle_geo_000F00),
            LOAD_MODEL_FROM_GEO(MODEL_GOOMBA,                      goomba_geo),

            AREA(/*index*/ 1, buildGeoLayout()),
                JUMP_LINK(buildObjects()),
                WARP_NODE(/*id*/ 0x00, /*destLevel*/ LEVEL_CASTLE_GROUNDS, /*destArea*/ 0x01, /*destNode*/ 0x00, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x01, /*destLevel*/ LEVEL_CASTLE_GROUNDS, /*destArea*/ 0x01, /*destNode*/ 0x01, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x02, /*destLevel*/ LEVEL_CASTLE_COURTYARD, /*destArea*/ 0x01, /*destNode*/ 0x01, /*flags*/ WARP_NO_CHECKPOINT),
                OBJECT(/*model*/ MODEL_CASTLE_STAR_DOOR_8_STARS, /*pos*/ -2706,   0, -1409, /*angle*/ 0,  0, 0, /*behParam*/ 0x08000000, /*beh*/ bhvStarDoor),
                OBJECT(/*model*/ MODEL_CASTLE_STAR_DOOR_8_STARS, /*pos*/ -2598,   0, -1517, /*angle*/ 0, 180, 0, /*behParam*/ 0x08000000, /*beh*/ bhvStarDoor),
                OBJECT(/*model*/ MODEL_CASTLE_KEY_DOOR,          /*pos*/ -1100,   0, -1074, /*angle*/ 0,   0, 0, /*behParam*/ 0x01030000, /*beh*/ bhvDoorWarp),
                OBJECT(/*model*/ MODEL_CASTLE_KEY_DOOR,          /*pos*/  -946,   0, -1074, /*angle*/ 0, 180, 0, /*behParam*/ 0x01040000, /*beh*/ bhvDoorWarp),
                OBJECT(/*model*/ MODEL_CASTLE_KEY_DOOR,          /*pos*/ -1100, 0,   922, /*angle*/ 0,   0, 0, /*behParam*/ 0x02050000, /*beh*/ bhvDoorWarp),
                OBJECT(/*model*/ MODEL_CASTLE_KEY_DOOR,          /*pos*/  -946, 0,   922, /*angle*/ 0, 180, 0, /*behParam*/ 0x02060000, /*beh*/ bhvDoorWarp),
                WARP_NODE(/*id*/ 0x03, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x02, /*destNode*/ 0x00, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x04, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x02, /*destNode*/ 0x01, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x05, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x03, /*destNode*/ 0x00, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x06, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x03, /*destNode*/ 0x01, /*flags*/ WARP_NO_CHECKPOINT),
                PAINTING_WARP_NODE(/*id*/ 0x00, /*destLevel*/ LEVEL_BOB, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                PAINTING_WARP_NODE(/*id*/ 0x01, /*destLevel*/ LEVEL_BOB, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                PAINTING_WARP_NODE(/*id*/ 0x02, /*destLevel*/ LEVEL_BOB, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                PAINTING_WARP_NODE(/*id*/ 0x03, /*destLevel*/ LEVEL_CCM, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                PAINTING_WARP_NODE(/*id*/ 0x04, /*destLevel*/ LEVEL_CCM, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                PAINTING_WARP_NODE(/*id*/ 0x05, /*destLevel*/ LEVEL_CCM, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                PAINTING_WARP_NODE(/*id*/ 0x06, /*destLevel*/ LEVEL_WF, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                PAINTING_WARP_NODE(/*id*/ 0x07, /*destLevel*/ LEVEL_WF, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                PAINTING_WARP_NODE(/*id*/ 0x08, /*destLevel*/ LEVEL_WF, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                PAINTING_WARP_NODE(/*id*/ 0x09, /*destLevel*/ LEVEL_JRB, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                PAINTING_WARP_NODE(/*id*/ 0x0A, /*destLevel*/ LEVEL_JRB, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                PAINTING_WARP_NODE(/*id*/ 0x0B, /*destLevel*/ LEVEL_JRB, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/  2013,  768, -2014, /*angle*/ 0,    0, 0, /*behParam*/ 0x000A0000, /*beh*/ bhvWarp),
                WARP_NODE(/*id*/ 0x0A, /*destLevel*/ LEVEL_PSS, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/ -5513,  512, -4324, /*angle*/ 0,    0, 0, /*behParam*/ 0x300B0000, /*beh*/ bhvWarp),
                WARP_NODE(/*id*/ 0x0B, /*destLevel*/ LEVEL_BITDW, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/  1963,  819,  1280, /*angle*/ 0,    0, 0, /*behParam*/ 0x050C0000, /*beh*/ bhvWarp),
                WARP_NODE(/*id*/ 0x0C, /*destLevel*/ LEVEL_SA, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0xF2, /*destLevel*/ LEVEL_TOTWC, /*destArea*/ 0x01, /*destNode*/ 0x0A, /*flags*/ WARP_NO_CHECKPOINT),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/ -1024,  512,  -650, /*angle*/ 0,    0, 0, /*behParam*/ 0x001E0000, /*beh*/ bhvInstantActiveWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/ -1024,  -50,   717, /*angle*/ 0,  180, 0, /*behParam*/ 0x001F0000, /*beh*/ bhvInstantActiveWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/ -1024,  900,   717, /*angle*/ 0,  180, 0, /*behParam*/ 0x00200000, /*beh*/ bhvAirborneWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/ -1024,  900,   717, /*angle*/ 0,  180, 0, /*behParam*/ 0x00210000, /*beh*/ bhvAirborneDeathWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/ -1024,  900,   717, /*angle*/ 0,  180, 0, /*behParam*/ 0x00220000, /*beh*/ bhvHardAirKnockBackWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/ -1024,  900,   717, /*angle*/ 0,  180, 0, /*behParam*/ 0x00230000, /*beh*/ bhvDeathWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/ -5513,  512, -4324, /*angle*/ 0, -135, 0, /*behParam*/ 0x00240000, /*beh*/ bhvLaunchStarCollectWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/ -5513,  512, -4324, /*angle*/ 0, -135, 0, /*behParam*/ 0x00250000, /*beh*/ bhvLaunchDeathWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/ -1024,  900,   717, /*angle*/ 0,    0, 0, /*behParam*/ 0x00260000, /*beh*/ bhvAirborneStarCollectWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/  2816, 1200,  -256, /*angle*/ 0,   90, 0, /*behParam*/ 0x00270000, /*beh*/ bhvAirborneStarCollectWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/  2816, 1200,  -256, /*angle*/ 0,  270, 0, /*behParam*/ 0x00280000, /*beh*/ bhvDeathWarp),
                WARP_NODE(/*id*/ 0x1E, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x1E, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x1F, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x1F, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x20, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x20, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x21, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x21, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x22, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x22, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x23, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x23, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x24, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x24, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x25, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x25, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x26, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x26, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x27, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x27, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x28, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x28, /*flags*/ WARP_NO_CHECKPOINT),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/ -5422, 717,  -461, /*angle*/ 0, 270, 0, /*behParam*/ 0x00320000, /*beh*/ bhvPaintingStarCollectWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/ -2304,   0, -4552, /*angle*/ 0, 180, 0, /*behParam*/ 0x00330000, /*beh*/ bhvPaintingStarCollectWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/   256, 102, -4706, /*angle*/ 0, 180, 0, /*behParam*/ 0x00340000, /*beh*/ bhvPaintingStarCollectWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/  4501, 717,  -230, /*angle*/ 0,  90, 0, /*behParam*/ 0x00350000, /*beh*/ bhvPaintingStarCollectWarp),
                WARP_NODE(/*id*/ 0x32, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x32, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x33, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x33, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x34, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x34, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x35, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x35, /*flags*/ WARP_NO_CHECKPOINT),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/ -5422, 717,  -461, /*angle*/ 0, 270, 0, /*behParam*/ 0x00640000, /*beh*/ bhvPaintingDeathWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/ -2304,   0, -4552, /*angle*/ 0, 180, 0, /*behParam*/ 0x00650000, /*beh*/ bhvPaintingDeathWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/   256, 102, -4706, /*angle*/ 0, 180, 0, /*behParam*/ 0x00660000, /*beh*/ bhvPaintingDeathWarp),
                OBJECT(/*model*/ MODEL_NONE, /*pos*/  4501, 717,  -230, /*angle*/ 0,  90, 0, /*behParam*/ 0x00670000, /*beh*/ bhvPaintingDeathWarp),
                WARP_NODE(/*id*/ 0x64, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x64, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x65, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x65, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x66, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x66, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0x67, /*destLevel*/ LEVEL_CASTLE, /*destArea*/ 0x01, /*destNode*/ 0x67, /*flags*/ WARP_NO_CHECKPOINT),
                WARP_NODE(/*id*/ 0xF1, /*destLevel*/ LEVEL_CASTLE_GROUNDS, /*destArea*/ 0x01, /*destNode*/ 0x03, /*flags*/ WARP_NO_CHECKPOINT),
                TERRAIN(/*terrainData*/ buildTerrain()),
                ROOMS(/*surfaceRooms*/ buildRooms()),
                MACRO_OBJECTS(/*objList*/ inside_castle_seg7_area_1_macro_objs),
                SET_BACKGROUND_MUSIC(/*settingsPreset*/ 0x0001, /*seq*/ SEQ_LEVEL_KOOPA_ROAD),
                TERRAIN_TYPE(/*terrainType*/ TERRAIN_STONE),
            END_AREA(),

            FREE_LEVEL_POOL(),
            MARIO_POS(/*area*/ 1, /*yaw*/ 180, /*pos*/ 0, 2000, 0),
            CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
            CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
            CLEAR_LEVEL(),
            SLEEP_BEFORE_EXIT(/*frames*/ 1),
            //CALL(/*arg*/ 0, /*func*/ freeLevelPool), FIXME ENGINE CRASHES RIGHT AFTER FREEING MEMORY
            RETURN()
        };

        auto newLevelScript = (LevelScript*)malloc(sizeof(scriptData));
        std::memcpy(newLevelScript, scriptData, sizeof(scriptData));

        return newLevelScript;
    }
public:
    static void setup() {
        "Hi";
    }

    static void addTriangleToBuildQueue(int t) {
        trianglesQueue.push(t);
    }

    static LevelScript* buildLevel();

};

#endif // SM64_PORT_LEVELSCRIPTMANAGER_H
