//
// Created by Luis Alvarez on 19/09/2026.
//

#include "StructuralFace.h"

#include "DisplayListManager.h"
#include "StructuralEdge.h"

#include <array>
#include <cassert>
#include <cmath>
#include <vector>

// TODO revisar todas estas redundancias
namespace {

using Tri = std::array<int, 3>;

// Earclipping sobre un polígono 3D (posiblemente no plano, posiblemente cóncavo).
// Entrada: posiciones en el orden cíclico del BoundaryLoop.
// Salida: triángulos como índices a `poly`, conservando el winding del loop.
// TODO reciclar estructuras de datos para no usar tanta memoria y procesamiento innecesarios
std::vector<Tri> earClip(const std::vector<Vector3>& poly) {
    const int n = static_cast<int>(poly.size());
    std::vector<Tri> out;
    if (n < 3) return out;
    out.reserve(n - 2);

    // 1. Plano de mejor ajuste (Newell): robusto ante caras no coplanares,
    //    que en esta arquitectura son estados válidos (contradicciones geométricas).
    double nx = 0, ny = 0, nz = 0;
    for (int i = 0; i < n; ++i) {
        const Vector3& a = poly[i];
        const Vector3& b = poly[(i + 1) % n];
        float ax = a.x, ay = a.y, az = a.z, bx = b.x, by = b.y, bz = b.z;
        nx += (ay - by) * (az + bz);
        ny += (az - bz) * (ax + bx);
        nz += (ax - bx) * (ay + by);
    }

    // 2. Proyección 2D descartando el eje dominante de la normal.
    //    Se usa permutación cíclica de ejes para conservar la quiralidad;
    //    el signo de la componente dominante dice si el polígono proyectado
    //    queda CCW (+1) o CW (-1), así no hace falta calcular el área con signo.
    std::vector<std::array<float, 2>> uv(n);
    float sign = 1.0f;
    double ax_ = std::abs(nx), ay_ = std::abs(ny), az_ = std::abs(nz);
    for (int i = 0; i < n; ++i) {
        float x = poly[i].x, y = poly[i].y, z = poly[i].z;
        if (ax_ >= ay_ && ax_ >= az_) { uv[i] = {y, z}; sign = nx >= 0 ? 1.0f : -1.0f; }
        else if (ay_ >= az_)          { uv[i] = {z, x}; sign = ny >= 0 ? 1.0f : -1.0f; }
        else                          { uv[i] = {x, y}; sign = nz >= 0 ? 1.0f : -1.0f; }
    }

    auto cross = [&](int a, int b, int c) {
        return ((uv[b][0] - uv[a][0]) * (uv[c][1] - uv[a][1]) -
                (uv[b][1] - uv[a][1]) * (uv[c][0] - uv[a][0])) * sign; // >0 => convexo
    };
    auto sameUV = [&](int a, int b) { return uv[a][0] == uv[b][0] && uv[a][1] == uv[b][1]; };
    auto inside = [&](int a, int b, int c, int p) {
        return cross(a, b, p) >= 0.0f && cross(b, c, p) >= 0.0f && cross(c, a, p) >= 0.0f;
    };

    // Normal degenerada (todos los vértices colineales): no hay nada que triangular bien.
    if (ax_ + ay_ + az_ < 1e-6) {
        for (int i = 1; i + 1 < n; ++i) out.push_back({0, i, i + 1});
        return out;
    }

    // 3. Clipping de orejas.
    std::vector<int> idx(n);
    for (int i = 0; i < n; ++i) idx[i] = i;

    while (idx.size() > 3) {
        const int m = static_cast<int>(idx.size());
        bool clipped = false;

        for (int k = 0; k < m && !clipped; ++k) {
            int a = idx[(k + m - 1) % m], b = idx[k], c = idx[(k + 1) % m];
            if (cross(a, b, c) <= 1e-6f) continue;          // reflejo o colineal

            bool ear = true;
            for (int j = 0; j < m && ear; ++j) {
                int p = idx[j];
                if (p == a || p == b || p == c) continue;
                if (sameUV(p, a) || sameUV(p, b) || sameUV(p, c)) continue; // vértice duplicado
                if (inside(a, b, c, p)) ear = false;
            }
            if (!ear) continue;

            out.push_back({a, b, c});
            idx.erase(idx.begin() + k);
            clipped = true;
        }

        if (!clipped) {
            // Sin orejas válidas (polígono auto-intersectado o casi degenerado):
            // se garantiza progreso recortando el vértice más cercano a colineal.
            int best = 0; float bestVal = std::abs(cross(idx[m - 1], idx[0], idx[1]));
            for (int k = 1; k < m; ++k) {
                float v = std::abs(cross(idx[(k + m - 1) % m], idx[k], idx[(k + 1) % m]));
                if (v < bestVal) { bestVal = v; best = k; }
            }
            out.push_back({idx[(best + m - 1) % m], idx[best], idx[(best + 1) % m]});
            idx.erase(idx.begin() + best);
        }
    }
    out.push_back({idx[0], idx[1], idx[2]});
    return out;
}

// Normal de Newell normalizada, con el mismo criterio de winding que earClip.
Vector3 faceNormal(const std::vector<Vector3>& poly) {
    double nx = 0, ny = 0, nz = 0;
    const int n = static_cast<int>(poly.size());
    for (int i = 0; i < n; ++i) {
        const Vector3& a = poly[i];
        const Vector3& b = poly[(i + 1) % n];
        float ax = a.x, ay = a.y, az = a.z, bx = b.x, by = b.y, bz = b.z;
        nx += (ay - by) * (az + bz);
        ny += (az - bz) * (ax + bx);
        nz += (ax - bx) * (ay + by);
    }
    return Vector3(static_cast<float>(nx), static_cast<float>(ny), static_cast<float>(nz)).normalized();
}

// TODO esto necesita su abstracción
const u8* textureFor(TriangleFaceType type) {
    switch (type) {
        case TriangleFaceType::CheckerboardFloor:     return inside_09004000;
        case TriangleFaceType::Lava:                  return fire_09009000;
        case TriangleFaceType::InsideWallFirstFloor:  return inside_09001000;
        case TriangleFaceType::InsideRoofFirstFloor:  return inside_09008000;
        case TriangleFaceType::InsideWallWood:        return inside_09003800;
        case TriangleFaceType::InsideWallBricks:      return inside_09003000;
        case TriangleFaceType::BasementPillar:        return inside_09007000;
        default:                                      return inside_09001000; // None: fallback visible
    }
}

// Texturas de 32x32 texels, coordenadas en s10.5.
constexpr float kTexSize = 32.0f;
constexpr float kFrac = 32.0f;

short toTexCoord(float texels) {
    return static_cast<short>(std::max(-32768.0f, std::min(32767.0f, std::round(texels * kFrac))));
}

// Base 2D sobre el plano de la cara. Suelos/techos: u = +X. Paredes: u horizontal a lo largo de la pared.
void planeBasis(const Vector3& n, Vector3& u, Vector3& v) {
    Vector3 up(0, 1, 0);
    float ny = n.y;
    if (std::abs(ny) > 0.99f) u = Vector3(1, 0, 0);
    else                      u = up.cross(n).normalized();
    v = n.cross(u).normalized();
}

} // namespace

extern "C" {
    #include "surface_terrains.h"
    #include "textures.h"
    #include "engine/surface_load.h"
}

namespace {

// sSurfacePool[0..kReservedSurfaces) son los placeholders que reserva buildTerrain()
// (2 del piso de seguridad + 64 del loop). collapseTerrain los reescribe en sitio.
constexpr int kReservedSurfaces = 66;

s16 toS16(float v) {
    return static_cast<s16>(std::max(-32768.0f, std::min(32767.0f, std::round(v))));
}

// Reproduce lo que hace el loader del motor al crear un Surface.
// Devuelve false si el triángulo es degenerado (el motor también lo descartaría).
bool writeSurface(Surface& s, const Vector3& a, const Vector3& b, const Vector3& c, s16 type) {
    s16 v[3][3] = {
        {toS16(a.x), toS16(a.y), toS16(a.z)},
        {toS16(b.x), toS16(b.y), toS16(b.z)},
        {toS16(c.x), toS16(c.y), toS16(c.z)},
    };
    float nx = (v[1][1] - v[0][1]) * (v[2][2] - v[1][2]) - (v[1][2] - v[0][2]) * (v[2][1] - v[1][1]);
    float ny = (v[1][2] - v[0][2]) * (v[2][0] - v[1][0]) - (v[1][0] - v[0][0]) * (v[2][2] - v[1][2]);
    float nz = (v[1][0] - v[0][0]) * (v[2][1] - v[1][1]) - (v[1][1] - v[0][1]) * (v[2][0] - v[1][0]);
    float mag = std::sqrt(nx * nx + ny * ny + nz * nz);
    if (mag < 0.0001f) return false;
    nx /= mag; ny /= mag; nz /= mag;

    s.type = type;
    for (int i = 0; i < 3; ++i) {
        s.vertex1[i] = v[0][i];
        s.vertex2[i] = v[1][i];
        s.vertex3[i] = v[2][i];
    }
    s.normal.x = nx; s.normal.y = ny; s.normal.z = nz;
    s.originOffset = -(nx * v[0][0] + ny * v[0][1] + nz * v[0][2]);
    s.lowerY = std::min(std::min(v[0][1], v[1][1]), v[2][1]) - 5;
    s.upperY = std::max(std::max(v[0][1], v[1][1]), v[2][1]) + 5;
    return true;
}

} // namespace

StructuralFace::~StructuralFace() {
    assert(false && "StructuralFace destructor called. This should not happen, as StructuralFaces are managed by "
                    "the system and should not be deleted manually.");
}
void StructuralFace::linkOuterLoop(BoundaryLoop& loop) {
    assert(outerLoop == nullptr
           && "StructuralFace::linkOuterLoop: outerLoop is already linked and its link is immutable.");
    if (outerLoop != nullptr) {
        throw NonRecoverableContradictionException(
            "StructuralFace outerLoop is already linked and its link is immutable.");
    }
    outerLoop = &loop;
}
BoundaryLoop& StructuralFace::getOuterLoop() const {
    assert(outerLoop != nullptr
           && "StructuralFace::getOuterLoop: StructuralFace has no associated outerLoop.");
    if (outerLoop == nullptr) {
        throw NonRecoverableContradictionException("StructuralFace has no associated outerLoop.");
    }
    return *outerLoop;
}
void StructuralFace::collapseTerrain() {
    // El polígono es el outerLoop de ESTA cara, ya no el pivotEdge global.
    std::vector<Vector3> poly;
    HalfEdge* start = &getOuterLoop().getFirstEdge();
    HalfEdge* h = start;
    do {
        poly.push_back({h->structuralEdge.displayVertex.position[0].get(),
                        h->structuralEdge.displayVertex.position[1].get(),
                        h->structuralEdge.displayVertex.position[2].get()});
        h = &h->walk();
    } while (h != start);

    const auto tris = earClip(poly);

    static int lastUsed = 0;   // slots escritos en el colapso anterior
    int slot = 0;
    for (const auto& t : tris) {
        if (slot >= kReservedSurfaces) {
            assert(false && "collapseTerrain: la cara excede los surfaces reservados en buildTerrain.");
            break;
        }
        if (writeSurface(sSurfacePool[slot], poly[t[0]], poly[t[1]], poly[t[2]], SURFACE_DEFAULT)) {
            ++slot;
        }
    }

    // Slots que quedaron huérfanos respecto al colapso anterior: se devuelven al placeholder.
    const Vector3 pa(1, 0, 1), pb(1, 0, -1), pc(-1, 0, -1);
    for (int i = slot; i < lastUsed; ++i) {
        writeSurface(sSurfacePool[i], pa, pb, pc, SURFACE_DEFAULT);
    }
    lastUsed = slot;
}
// TODO reciclar estructuras de datos para no usar tanta memoria y procesamiento innecesarios
void StructuralFace::collapseDisplayList() {
    // 1. Polígono del outerLoop de ESTA cara, en orden cíclico.
    std::vector<Vector3> poly;
    HalfEdge* start = &getOuterLoop().getFirstEdge();
    HalfEdge* h = start;
    do {
        poly.push_back({h->structuralEdge.displayVertex.position[0].get(),
                        h->structuralEdge.displayVertex.position[1].get(),
                        h->structuralEdge.displayVertex.position[2].get()});
        h = &h->walk();
    } while (h != start);

    assert (poly.size() >= 3 && "StructuralFace::collapseDisplayList: outerLoop must have at least 3 vertices.");

    // 2. Misma triangulación que collapseTerrain: render y colisión no pueden divergir.
    const auto tris = earClip(poly);
    assert(!tris.empty() && "StructuralFace::collapseDisplayList: triangulation failed.");

    // 3. Atributos por cara (normal, UV): viven en la copia del vértice del segmento,
    //    no en structuralEdge.displayVertex, porque esa arista la comparten dos caras.
    const Vector3 normal = faceNormal(poly);
    Vector3 uAxis, vAxis;
    planeBasis(normal, uAxis, vAxis);

    const int n = static_cast<int>(poly.size());
    std::vector<float> us(n), vs(n);
    float uMin = 1e30f, uMax = -1e30f, vMin = 1e30f, vMax = -1e30f;
    for (int i = 0; i < n; ++i) {
        us[i] = poly[i].dot(uAxis);
        vs[i] = poly[i].dot(vAxis);
        uMin = std::min(uMin, us[i]); uMax = std::max(uMax, us[i]);
        vMin = std::min(vMin, vs[i]); vMax = std::max(vMax, vs[i]);
    }

    // 4. UV según uvConfig. Se trabaja relativo al mínimo para no desbordar el s10.5 en caras grandes.
    std::vector<DisplayVertex*> vertices;
    vertices.reserve(n);
    h = start;
    for (int i = 0; i < n; ++i) {
        float tu = 0.0f, tv = 0.0f; // en texels
        if (const auto* rep = std::get_if<RepeatUV>(&uvConfig.mode)) {
            float w = std::max<float>(1.0f, rep->tileWidth);
            float hgt = std::max<float>(1.0f, rep->tileHeight);
            tu = (us[i] - uMin) / w * kTexSize;
            tv = (vs[i] - vMin) / hgt * kTexSize;
        } else if (const auto* st = std::get_if<StretchUV>(&uvConfig.mode)) {
            float du = std::max(uMax - uMin, 1e-3f);
            float dv = std::max(vMax - vMin, 1e-3f);
            float a = static_cast<float>(st->startPixel), b = static_cast<float>(st->endPixel);
            tu = a + (us[i] - uMin) / du * (b - a);
            tv = a + (vs[i] - vMin) / dv * (b - a);
        }

        h = &h->walk();
        DisplayVertex* dv = &h->structuralEdge.displayVertex;
        dv->position[0].set(poly[i].x);
        dv->position[1].set(poly[i].y);
        dv->position[2].set(poly[i].z);
        dv->flag = 0;
        dv->textureU = toTexCoord(tu);
        dv->textureV = toTexCoord(tv);
        dv->normal = normal;
        vertices.push_back(dv);
    }

    std::vector<std::array<int, 3>> triangles(tris.begin(), tris.end());

    DisplayListManager::addDisplayListSegment(vertices, triangles, textureFor(faceType));
}
