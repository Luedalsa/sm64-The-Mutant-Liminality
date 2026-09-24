//
// Created by Luis Alvarez on 21/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_UVMODES_H
#define SM64_THE_MUTANT_LIMINALITY_UVMODES_H
#include <cstdint>
#include <variant>

struct RepeatUV {
    uint32_t tileWidth;
    uint32_t tileHeight;
};

struct StretchUV {
    uint32_t startPixel;
    uint32_t endPixel;
};

struct SurfaceUVConfig {
    std::variant<RepeatUV, StretchUV> mode;
};

#endif // SM64_THE_MUTANT_LIMINALITY_UVMODES_H
