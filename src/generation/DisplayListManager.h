//
// Created by Luis Alvarez on 03/07/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_DISPLAYLISTMANAGER_H
#define SM64_THE_MUTANT_LIMINALITY_DISPLAYLISTMANAGER_H

#include "LevelPoolManager.h"
#include "TriangleCollection.h"
#include "textures.h"

#include <cstring>

#include <vector>
#include <stdexcept>

// SM64: The Mutant Liminality. Procedural Geometry Generation.
// I think this is a good name

extern "C" {
#include <ultra64.h>

#include "level_commands.h"

} // extern "C"

static constexpr Lights1 globalLights = gdSPDefLights1(
    0x5f, 0x5f, 0x5f,
    0xff, 0xff, 0xff, 0x28, 0x28, 0x28
);


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
    constexpr size_t maxVertsPerLoad = 32;

    struct Chunk {
        std::vector<DisplayVertex> verts;
        std::vector<std::array<int, 3>> tris;
    };
    std::vector<Chunk> chunks;
    chunks.emplace_back();

    std::vector<int> remap(vertices.size(), -1);

    for (const auto& tri : triangles) {
        int missing = 0;
        for (int i = 0; i < 3; ++i) {
            if (remap[tri[i]] == -1) missing++;
        }

        if (chunks.back().verts.size() + missing > maxVertsPerLoad) {
            chunks.emplace_back();
            std::fill(remap.begin(), remap.end(), -1);
        }

        Chunk& current = chunks.back();
        std::array<int, 3> localTri{};
        for (int i = 0; i < 3; ++i) {
            int orig = tri[i];
            if (remap[orig] == -1) {
                current.verts.push_back(vertices[orig]);
                remap[orig] = static_cast<int>(current.verts.size()) - 1;
            }
            localTri[i] = remap[orig];
        }
        current.tris.push_back(localTri);
    }

    const size_t headerCount = 5; // gsDPSetTextureImage, gsDPLoadSync, gsDPLoadBlock, gsSPLight x2
    const size_t footerCount = 1; // gsSPEndDisplayList
    size_t bodyCount = 0;
    for (const auto& chunk : chunks) {
        bodyCount += 1 /*gsSPVertex*/ + chunk.tris.size();
    }

    Gfx* displayListSegment = LevelPoolManager::allocOnPool<Gfx>(headerCount + bodyCount + footerCount);

    auto p = displayListSegment;

    *p++ = gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture);
    *p++ = gsDPLoadSync();
    *p++ = gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES));
    //*p++ = gsSPLight(&globalLights.l, 1);
    //*p++ = gsSPLight(&globalLights.a, 2);

    for (const auto& chunk : chunks) {
        *p++ = gsSPVertex(buildVertexSegment(chunk.verts), chunk.verts.size(), 0);
        for (const auto& triangle : chunk.tris) {
            *p++ = gsSP1Triangle(triangle[0], triangle[1], triangle[2], 0x0);
        }
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
        //*p++ = gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE);
        //*p++ = gsSPSetGeometryMode(G_LIGHTING);
        *p++ = gsSPEndDisplayList();

        return displayList;
    }

};


#endif // SM64_THE_MUTANT_LIMINALITY_DISPLAYLISTMANAGER_H
