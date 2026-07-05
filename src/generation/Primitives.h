//
// Created by Luis Alvarez on 04/07/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_PRIMITIVES_H
#define SM64_THE_MUTANT_LIMINALITY_PRIMITIVES_H

#include <types.h>
#include <cstdint>
#include <cmath>
#include <iostream>

template <typename T>
struct Axis {
private:
    float v = 0.0f; // Always float. Template is for operators.
public:
    Axis(T nv) : v(nv) {
        if (nv > INT16_MAX || nv < INT16_MIN) {
            std::cerr << "Axis component must be in the range of int16_t. Value will be truncated on getter. " << std::endl;
        }
    }

    operator T() const { return v; }
    operator short int() const {
        if (v > INT16_MAX || v < INT16_MIN) {
            std::cerr << "Axis component must be in the range of int16_t. VALUE HAS BEEN TRUNCATED." << std::endl;
        }
        return v;
    }

    Axis& operator=(float n) {
        if (v > INT16_MAX || v < INT16_MIN) {
            std::cerr << "Axis component must be in the range of int16_t. Value will be truncated on getter. " << std::endl;
        }
        v = n;
        return *this;
    }

    // Unary
    Axis operator-() const { return Axis(-v); }
    Axis operator+() const { return Axis(v); }

    // Axis <op> Axis
    Axis operator+(const Axis& d) const { return Axis(v + d.v); }
    Axis operator-(const Axis& d) const { return Axis(v - d.v); }
    Axis operator*(const Axis& d) const { return Axis(v * d.v); }
    Axis operator/(const Axis& d) const { return Axis(v / d.v); }

    // Axis <op> T
    Axis operator+(T d) const { return Axis(v + d); }
    Axis operator-(T d) const { return Axis(v - d); }
    Axis operator*(T d) const { return Axis(v * d); }
    Axis operator/(T d) const { return Axis(v / d); }

    // Compound assignment
    Axis& operator+=(T d) { return *this = v + d; }
    Axis& operator-=(T d) { return *this = v - d; }
    Axis& operator*=(T d) { return *this = v * d; }
    Axis& operator/=(T d) { return *this = v / d; }

    Axis& operator+=(const Axis& d) { return *this = v + d.v; }
    Axis& operator-=(const Axis& d) { return *this = v - d.v; }
    Axis& operator*=(const Axis& d) { return *this = v * d.v; }
    Axis& operator/=(const Axis& d) { return *this = v / d.v; }

    // Comparisons
    bool operator==(const Axis& d) const { return v == d.v; }
    bool operator!=(const Axis& d) const { return v != d.v; }
    bool operator<(const Axis& d)  const { return v < d.v; }
    bool operator>(const Axis& d)  const { return v > d.v; }
    bool operator<=(const Axis& d) const { return v <= d.v; }
    bool operator>=(const Axis& d) const { return v >= d.v; }

    friend std::ostream& operator<<(std::ostream& os, const Axis& a) {
        return os << a.v;
    }
};

struct SurfaceTransform {
    int triangle = -1;
    float u = 0.5;
    float v = 0.5;
    Vec3f normalDirection{0, 1, 0};
};

#endif // SM64_THE_MUTANT_LIMINALITY_PRIMITIVES_H
