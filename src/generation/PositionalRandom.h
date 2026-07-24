//
// Created by Luis Alvarez on 23/07/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_POSITIONALRANDOM_H
#define SM64_THE_MUTANT_LIMINALITY_POSITIONALRANDOM_H

#include "Primitives.h"

#include <cstdint>
#include <cmath>

class PositionalRandom {
public:
    struct QuantizedPos {
        int32_t x, y, z;
    };

    static QuantizedPos quantize(const Vector3& p, float epsilon = 1.0f) {
        return QuantizedPos{
            static_cast<int32_t>(std::round(static_cast<float>(p.x) / epsilon)),
            static_cast<int32_t>(std::round(static_cast<float>(p.y) / epsilon)),
            static_cast<int32_t>(std::round(static_cast<float>(p.z) / epsilon))
        };
    }

    static uint32_t hash32(uint32_t seed, const QuantizedPos& pos, uint32_t channel = 0) {
        constexpr uint32_t BIT_NOISE1 = 0xB5297A4Du;
        constexpr uint32_t BIT_NOISE2 = 0x68E31DA4u;
        constexpr uint32_t BIT_NOISE3 = 0x1B56C4E9u;

        auto mangle = [](uint32_t n, uint32_t s) -> uint32_t {
            n *= BIT_NOISE1;
            n += s;
            n ^= (n >> 8);
            n += BIT_NOISE2;
            n ^= (n << 8);
            n *= BIT_NOISE3;
            n ^= (n >> 8);
            return n;
        };

        uint32_t h = seed;
        h = mangle(static_cast<uint32_t>(pos.x), h);
        h = mangle(static_cast<uint32_t>(pos.y) ^ 0x9E3779B9u, h);
        h = mangle(static_cast<uint32_t>(pos.z) ^ 0x85EBCA6Bu, h);
        h = mangle(channel ^ 0xC2B2AE35u, h);
        return h;
    }

    static uint32_t hash32(uint32_t seed, const Vector3& worldPos, uint32_t channel = 0, float epsilon = 1.0f) {
        return hash32(seed, quantize(worldPos, epsilon), channel);
    }

    // Float determinista en [0, 1).
    static float next01(uint32_t seed, const Vector3& worldPos, uint32_t channel = 0, float epsilon = 1.0f) {
        uint32_t h = hash32(seed, worldPos, channel, epsilon);
        return static_cast<float>(h >> 8) / static_cast<float>(1u << 24);
    }

    static bool chance(uint32_t seed, const Vector3& worldPos, float probability, uint32_t channel = 0, float epsilon = 1.0f) {
        return next01(seed, worldPos, channel, epsilon) < probability;
    }

    static int nextInt(uint32_t seed, const Vector3& worldPos, int exclusiveMax, uint32_t channel = 0, float epsilon = 1.0f) {
        if (exclusiveMax <= 0) return 0;
        uint32_t h = hash32(seed, worldPos, channel, epsilon);
        return static_cast<int>(h % static_cast<uint32_t>(exclusiveMax));
    }

    static int weightedPick(uint32_t seed, const Vector3& worldPos, const float* weights, int count, uint32_t channel = 0, float epsilon = 1.0f) {
        float total = 0.0f;
        for (int i = 0; i < count; ++i) total += weights[i];
        if (total <= 0.0f) return -1;

        float roll = next01(seed, worldPos, channel, epsilon) * total;
        float acc = 0.0f;
        for (int i = 0; i < count; ++i) {
            acc += weights[i];
            if (roll < acc) return i;
        }
        return count - 1;
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_POSITIONALRANDOM_H
