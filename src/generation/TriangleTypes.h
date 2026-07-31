//
// Created by Luis Alvarez on 30/07/2026.
//
#ifndef SM64_THE_MUTANT_LIMINALITY_TRIANGLETYPES_H
#define SM64_THE_MUTANT_LIMINALITY_TRIANGLETYPES_H

#include <vector>
#include <functional>
#include <array>
#include <cstdint>

enum class TriangleFaceType : uint8_t {
    None = 0,
    CheckerboardFloor,
    Lava,
    Count
};

using BuildGeometryFn = std::function<void(std::vector<int>& neighbours)>;

class TriangleTypeRegistry {
    static std::array<BuildGeometryFn, static_cast<size_t>(TriangleFaceType::Count)> table;
public:
    static void registerType(TriangleFaceType type, BuildGeometryFn fn) {
        table[static_cast<size_t>(type)] = std::move(fn);
    }

    static void build(TriangleFaceType type, std::vector<int>& neighbours) {
        auto& fn = table[static_cast<size_t>(type)];
        if (fn) fn(neighbours);
    }
};

void registerBuiltinTriangleTypes();

#endif // SM64_THE_MUTANT_LIMINALITY_TRIANGLETYPES_H