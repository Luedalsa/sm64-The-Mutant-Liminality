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

    friend class Vector3;
    Axis(T nv) : v(nv) {
        if (nv > INT16_MAX || nv < INT16_MIN) {
            std::cerr << "Axis component must be in the range of int16_t. Value will be truncated on getter. " << std::endl;
        }
    }
public:

    operator T() const { return v; }
    operator short int() const {
        if (v > INT16_MAX || v < INT16_MIN) {
            std::cerr << "Axis component must be in the range of int16_t. VALUE HAS BEEN TRUNCATED." << std::endl;
        }
        return v;
    }
    operator signed char() const {
        if (v > INT8_MAX || v < INT8_MIN) {
            std::cerr << "Axis component must be in the range of int8_t. VALUE HAS BEEN TRUNCATED." << std::endl;
        }
        return v;
    }

    Axis& operator=(float n) {
        if (n > INT16_MAX || n < INT16_MIN) {
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

struct Vector3 {
    Axis<float> x, y, z;

    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    static Vector3 zero() { return Vector3(0, 0, 0); }

    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3 operator/(float scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    Vector3& operator+=(const Vector3& other) { return *this = *this + other; }
    Vector3& operator-=(const Vector3& other) { return *this = *this - other; }
    Vector3& operator*=(float scalar) { return *this = *this * scalar; }
    Vector3& operator/=(float scalar) { return *this = *this / scalar; }

    bool operator==(const Vector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    bool operator!=(const Vector3& other) const { return !(*this == other); }

    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3 cross(const Vector3& other) const {
        auto vector = Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
        return vector;
    }

    double length() const {
        return std::sqrt(static_cast<double>(x * x + y * y + z * z));
    }

    Vector3 normalized() const {
        double len = length();
        if (len == 0.0) {
            return Vector3::zero();
        }
        auto vector = Vector3(
            static_cast<float>(x / len),
            static_cast<float>(y / len),
            static_cast<float>(z / len)
        );
        return vector;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
        return os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    }
};

struct SurfaceTransform {
    int triangle = -1;
    float u = 0.5;
    float v = 0.5;
    Vector3 forward{0, 0, -1};

    Vector3 getUVPosition() const;
};

struct DisplayVertex {
    Vector3 position;
    short int flag = 0;
    short int textureU;
    short int textureV;
    Vector3 normal;

    operator Vtx() const {
        Vtx vtx{};
        vtx.n.ob[0] = static_cast<short>(position.x); // Axis ya trunca/avisa
        vtx.n.ob[1] = static_cast<short>(position.y);
        vtx.n.ob[2] = static_cast<short>(position.z);
        vtx.n.flag  = static_cast<unsigned short>(flag);
        vtx.n.tc[0] = textureU;
        vtx.n.tc[1] = textureV;
        vtx.n.n[0]  = static_cast<signed char>(normal.x * 127.0f);
        vtx.n.n[1]  = static_cast<signed char>(normal.y * 127.0f);
        vtx.n.n[2]  = static_cast<signed char>(normal.z * 127.0f);
        vtx.n.a     = 0xFF;
        return vtx;
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_PRIMITIVES_H
