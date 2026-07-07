//
// Created by Luis Alvarez on 16/06/2026.
//

#include "LevelScriptManager.h"
#include "VertexCollection.h"
#include "AbstractTriangle.h"
#include "AbstractVertex.h"


// ─────────────────────────────────────────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────────────────────────────────────────
namespace {

// find_vector_perpendicular_to_plane devuelve el producto vectorial crudo
// (no normalizado). Con coordenadas SM64 (~±8192) su magnitud es ~10^7.
// Usamos un epsilon relativo: dos puntos se consideran "en el plano" cuando
// |d| < REL_EPS * |normal|^2  (escala cuadrática porque d ya tiene una
// multiplicación extra respecto de la distancia euclidiana real).
// En la práctica 0.5 unidades de mundo es suficiente tolerancia.
constexpr float ON_PLANE_EPS_WORLD = 0.5f;   // distancia euclidiana mínima

// Distancia con signo de un punto al plano definido por (normal, planePoint).
// Con normal no normalizado, el valor NO es distancia euclidiana; el signo sí
// es correcto. Para comparaciones relativas usamos el cociente d/|normal|.
inline float signedDist(const Vec3f normal,
                        const Vec3f planePoint,
                        const Vec3f queryPoint)
{
    return normal[0]*(queryPoint[0] - planePoint[0])
         + normal[1]*(queryPoint[1] - planePoint[1])
         + normal[2]*(queryPoint[2] - planePoint[2]);
}

// |normal|  (una sola vez por llamada a clip)
inline float normalLen(const Vec3f n) {
    return sqrtf(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
}

// Interpola P = A + t*(B - A),  t ∈ [0,1]
inline AbstractVertex* lerpVertex(const AbstractVertex* a,
                                  const AbstractVertex* b,
                                  float t)
{/*
    auto* v = new AbstractVertex(
    a->position[0] + t * (b->position[0] - a->position[0]),
    a->position[1] + t * (b->position[1] - a->position[1]),
    a->position[2] + t * (b->position[2] - a->position[2])
        );
    return v;*/
}

} // namespace anónimo


AbstractTriangle::AbstractTriangle(int v1,
                                   int v2,
                                   int v3)
{
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;

    const auto & a = VertexCollection::getVertex(v1)->position;
    const auto & b = VertexCollection::getVertex(v2)->position;
    const auto & c = VertexCollection::getVertex(v3)->position;

    const Vector3 u = b - a;
    const Vector3 v = c - a;

    normal = u.cross(v).normalized();
}

void AbstractTriangle::build() {
    if (built) return;
    built = true;

    std::vector<int> trianglesToBuild;
    std::queue<int> frontier;

    for (int i = 0; i < 3; ++i) {
        for (auto t : VertexCollection::getVertex(vertices[i])->connectingTriangles) {
            frontier.push(t);
        }
    }

    while (!frontier.empty()) {
        int t = frontier.front();
        frontier.pop();

        AbstractTriangle* neighbor = TriangleCollection::getTriangle(t);

        if (neighbor == this) continue;

        if (typeid(*neighbor) == typeid(*this)) {
            if (neighbor->built) continue;
            neighbor->built = true;
            trianglesToBuild.push_back(t);

            for (int i = 0; i < 3; ++i) {
                for (auto t2 : VertexCollection::getVertex(neighbor->vertices[i])->connectingTriangles) {
                    frontier.push(t2);
                }
            }
        } else {
            LevelScriptManager::addTriangleToBuildQueue(t);
        }
    }

    buildGeometry(trianglesToBuild);
}


    std::vector<AbstractTriangle*> AbstractTriangle::clip(const AbstractTriangle& cutter) const
    {/* // TODO: IMPLEMENT CORRECTLY
        const Vec3f& N  = cutter.normal;
        const Vec3f& P0 = VertexCollection::getVertex(cutter.vertices[0])->position;

        // ── Paso 1: distancias con signo y clasificación ─────────────────────
        // Usamos la distancia euclidiana normalizada para el epsilon,
        // pero guardamos los d "crudos" para calcular t (la normalización
        // se cancela en el cociente d[i]/(d[i]-d[j]), así que no hace falta).
        const float nLen = normalLen(N);
        // Evitar división por cero en degenerate (normal nula)
        if (nLen < 1e-10f) {
            return { new AbstractTriangle(vertices[0], vertices[1], vertices[2]) };
        }
        const float eps = ON_PLANE_EPS_WORLD * nLen;   // umbral en unidades "crudas"

        float d[3];
        int   sign[3];
        int   posCount = 0, negCount = 0;

        for (int i = 0; i < 3; ++i) {
            d[i] = signedDist(N, P0, VertexCollection::getVertex(vertices[i])->position);
            if      (d[i] >  eps) { sign[i] = +1; ++posCount; }
            else if (d[i] < -eps) { sign[i] = -1; ++negCount; }
            else                  { sign[i] =  0; }
        }

        // ── Paso 2: sin intersección real ────────────────────────────────────
        if (posCount == 0 || negCount == 0) {
            return { new AbstractTriangle(vertices[0], vertices[1], vertices[2]) };
        }

        // ── Paso 3: corte genérico (posCount+negCount == 3) ──────────────────
        //    Hay exactamente 1 vértice en un lado y 2 en el otro.
        //    Buscamos ese vértice "solitario".
        if (posCount + negCount == 3) {
            // Solitario: el que está solo en su lado (sign diferente a los otros dos)
            int solo = -1;
            {
                int targetSign = (posCount == 1) ? +1 : -1;
                for (int i = 0; i < 3; ++i) {
                    if (sign[i] == targetSign) { solo = i; break; }
                }
            }
            assert(solo != -1);

            // Reindexamos ciclicamente: solo→0, siguiente→1, siguiente→2
            const int i0 = solo;
            const int i1 = (solo + 1) % 3;
            const int i2 = (solo + 2) % 3;

            const int V0 = (vertices[i0]);  // solitario
            const int V1 = (vertices[i1]);  // lado opuesto
            const int V2 = (vertices[i2]);  // lado opuesto

            // Puntos de corte sobre aristas V0→V1 y V0→V2
            // t = d[A] / (d[A] - d[B])  con A=solitario, B=opuesto
            // Garantizamos d[i0] y d[i1] tienen signo opuesto (y d[i0] y d[i2])
            const float denom01 = d[i0] - d[i1];
            const float denom02 = d[i0] - d[i2];

            // Ambos denominadores son != 0 porque los signos son opuestos
            const float t01 = d[i0] / denom01;
            const float t02 = d[i0] / denom02;

            AbstractVertex* P01 = lerpVertex(V0, V1, t01);
            AbstractVertex* P02 = lerpVertex(V0, V2, t02);

            // ┌────────────── Retriangulación ──────────────────────────┐
            //           V0  (solitario)
            //          /  \
            //        P01  P02   ← línea de corte
            //        / ╲ ╱ \
            //      V1   X   V2
            //
            //  T_solo  = (V0,  P01, P02)
            //  T_quad1 = (P01, V1,  V2 )
            //  T_quad2 = (P01, V2,  P02)        ← winding preservado
            // └────────────────────────────────────────────────────────┘
            return {
                new AbstractTriangle(V0,  P01, P02),
                new AbstractTriangle(P01, V1,  V2),
                new AbstractTriangle(P01, V2,  P02)
            };
        }

        // ── Paso 4: un vértice exactamente en el plano (posCount+negCount==2) ─
        //    sign[i]==0 para exactamente un i; los otros dos son +1 y -1.
        for (int i = 0; i < 3; ++i) {
            if (sign[i] == 0) {
                const int j = (i + 1) % 3;
                const int k = (i + 2) % 3;
                // La arista j→k cruza el plano (sign[j] y sign[k] son opuestos)
                const float denom = d[j] - d[k];
                // denom != 0 porque sign[j] != sign[k]
                const float t = d[j] / denom;
                AbstractVertex* mid = lerpVertex(vertices[j], vertices[k], t);

                // ┌─── Retriangulación ─────────────────────────────────┐
                //   vertices[i] está en el plano → vértice de la arista de corte
                //   T1 = (Vi, Vj, mid)
                //   T2 = (Vi, mid, Vk)
                // └────────────────────────────────────────────────────┘
                return {
                    new AbstractTriangle(vertices[i], vertices[j], mid),
                    new AbstractTriangle(vertices[i], mid,         vertices[k])
                };
            }
        }

        // No debería llegar aquí (todos los casos están cubiertos)
        return { new AbstractTriangle(vertices[0], vertices[1], vertices[2]) };*/
    }
/*
    // ── Utilidad: ¿el plano de `other` intersecta este triángulo? ───────────
    bool intersectsPlaneOf(const AbstractTriangle& other) const {
        const Vec3f& N  = other.normal;
        const Vec3f& P0 = other.vertices[0]->position;
        const float  nLen = normalLen(N);
        if (nLen < 1e-10f) return false;
        const float eps = ON_PLANE_EPS_WORLD * nLen;
        float d0 = signedDist(N, P0, vertices[0]->position);
        float d1 = signedDist(N, P0, vertices[1]->position);
        float d2 = signedDist(N, P0, Vvertices[2]->position);
        bool anyPos = d0 > eps || d1 > eps || d2 > eps;
        bool anyNeg = d0 < -eps || d1 < -eps || d2 < -eps;
        return anyPos && anyNeg;
    }*/

void CheckerboardFloorTriangle::buildGeometry(std::vector<int>& neighbours) {
    AbstractVertex* vertexa = VertexCollection::getVertex(getVertex(0));
    AbstractVertex* vertexb = VertexCollection::getVertex(getVertex(1));
    AbstractVertex* vertexc = VertexCollection::getVertex(getVertex(2));
    int vv1 = vertexa->getCollisionVertex();
    int vv2 = vertexb->getCollisionVertex();
    int vv3 = vertexc->getCollisionVertex();
    CollisionManager::addCollisionTriangle(vv1, vv2, vv3, SURFACE_DEFAULT);
}