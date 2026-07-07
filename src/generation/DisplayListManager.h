//
// Created by Luis Alvarez on 03/07/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_DISPLAYLISTMANAGER_H
#define SM64_THE_MUTANT_LIMINALITY_DISPLAYLISTMANAGER_H

#include "CollisionManager.h"
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


class DisplayListManager {
    static std::vector<std::vector<DisplayVertex>> displayVertices;
    static std::vector<std::vector<std::array<int, 3>>> displayTriangles;
    static std::vector<const u8*> displayTextures;

    static Vtx* buildVertexSegment(const std::vector<DisplayVertex>& vertices) {
        if (vertices.size() > 32) {
            throw std::length_error("Vertex segment exceeds maximum size of 32 vertices.");
        }

        Vtx* vertexSegment = LevelPoolManager::allocOnPool<Vtx>(vertices.size());
        for (size_t i = 0; i < vertices.size(); i++) {
            vertexSegment[i] = vertices[i];
        }

        return vertexSegment;
    }

    static Gfx* buildDisplayListSegment(const std::vector<DisplayVertex>& vertices, const std::vector<std::array<int, 3>>& triangles, const u8* texture) {
        const size_t headerCount = 4;
        const size_t footerCount = 1;
        const size_t triangleCount = triangles.size();

        Gfx* displayListSegment = LevelPoolManager::allocOnPool<Gfx>(headerCount + triangleCount + footerCount);

        auto p = displayListSegment;

        *p++ = gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture);
        *p++ = gsDPLoadSync();
        *p++ = gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES));
        *p++ = gsSPVertex(buildVertexSegment(vertices), vertices.size(), 0);

        for (const auto& triangle : triangles) {
            *p++ = gsSP1Triangle(triangle[0], triangle[1], triangle[2], 0x0);
        }

        *p++ = gsSPEndDisplayList();

        return displayListSegment;
    }

public:

    static void addDisplayListSegment(const std::vector<DisplayVertex>& vertices, const std::vector<std::array<int, 3>>& triangles, const u8* texture) {
        displayVertices.push_back(vertices);
        displayTriangles.push_back(triangles);
        displayTextures.push_back(texture);
    }

    static Gfx* buildDisplayList() {
        const size_t headerCount = 64; // TODO please do proper counting
        const size_t footerCount = 64;
        const size_t listsCount = displayVertices.size();

        Gfx* displayList = LevelPoolManager::allocOnPool<Gfx>(headerCount + listsCount + footerCount);

        auto p = displayList;

        *p++ = gsDPPipeSync();
        *p++ = gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB);
        *p++ = gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD);
        *p++ = gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);

        *p++ = gsDPTileSync();
        *p++ = gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD);
        *p++ = gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC);

        for (int i = 0; i < listsCount; i++) {
            *p++ = gsSPDisplayList(buildDisplayListSegment(displayVertices[i], displayTriangles[i], displayTextures[i]));
        }

        /**p++ = gsSPDisplayList(inside_castle_seg7_dl_07028418);
        *p++ = gsSPDisplayList(inside_castle_seg7_dl_070286C0),;*/

        *p++ = gsDPTileSync();
        *p++ = gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD);
        *p++ = gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC);

        *p++ = gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
        *p++ = gsDPPipeSync();
        *p++ = gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE);
        *p++ = gsSPEndDisplayList();

        return displayList;
    }

};


#endif // SM64_THE_MUTANT_LIMINALITY_DISPLAYLISTMANAGER_H
