//
// Created by Luis Alvarez on 04/07/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_PRIMITIVES_H
#define SM64_THE_MUTANT_LIMINALITY_PRIMITIVES_H

#include <types.h>

struct Transform {
    int triangle = -1;
    float u = 0.5;
    float v = 0.5;
    Vec3f normalDirection{0, 1, 0};
};

#endif // SM64_THE_MUTANT_LIMINALITY_PRIMITIVES_H
