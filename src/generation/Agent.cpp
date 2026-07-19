//
// Created by Luis Alvarez on 19/06/2026.
//

#include "Agent.h"

void DebugAgent2::grow() {
    energy--;

    float fy = birthPosition.forward.y;
    constexpr float EPS = 0.001f;
    float cosPitch = std::sqrt(std::max(0.0f, 1.0f - fy * fy));
    float frontHeight = (cosPitch > EPS) ? (fy * 3000.0f / cosPitch) : std::copysign(30000.0f, fy);

    int v4 = createLocalVertex({ -3000, frontHeight, -3000 }); // Frente, Izquierda
    int v2 = createLocalVertex({ 3000, frontHeight, -3000 });  // Frente, Derecha
    int v3 = createLocalVertex({ -3000, 0, 0 });               // Atrás, Izquierda
    int v1 = createLocalVertex({ 3000, 0, 0 });                // Atrás, Derecha

    int v6 = createLocalVertex({ -3000, frontHeight + 3000.0f, -3000 }); // Frente, Izquierda
    int v5 = createLocalVertex({ 3000, frontHeight + 3000.0f, -3000 });  // Frente, Derecha
    int v7 = createLocalVertex({ -3000, 3000, 0 });                      // Atrás, Izquierda
    int v8 = createLocalVertex({ 3000, 3000, 0 });                       // Atrás, Derecha

    // --- TRIÁNGULOS ---
    // Cara Inferior (Base)

    TriangleCollection::createTriangle<LavaTriangle>(v1, v2, v3);
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v3, v2, v4);

    // Cara Frontal (Z = -3000)
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v4, v2, v5);
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v4, v5, v6);

    // Cara Superior (Techo)
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v6, v5, v8);
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v6, v8, v7);

    // Cara Trasera (Z = 3000)
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v1, v3, v7);
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v1, v7, v8);

    // Cara Izquierda (X = -3000)
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v3, v4, v6);
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v3, v6, v7);

    // Cara Derecha (X = 3000)
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v2, v1, v8);
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v2, v8, v5);
}

void DebugAgent::grow() {
    energy--;

    float fy = birthPosition.forward.y;
    constexpr float EPS = 0.001f;
    float cosPitch = std::sqrt(std::max(0.0f, 1.0f - fy * fy));
    float frontHeight = (cosPitch > EPS) ? (fy * 3000.0f / cosPitch) : std::copysign(30000.0f, fy);

    int v4 = createLocalVertex({ -1000, frontHeight, -1000 }); // Frente, Izquierda
    int v2 = createLocalVertex({ 1000, frontHeight, -1000 });  // Frente, Derecha
    int v3 = createLocalVertex({ -1000, 0, 0 });               // Atrás, Izquierda
    int v1 = createLocalVertex({ 1000, 0, 0 });                // Atrás, Derecha

    int v6 = createLocalVertex({ -1000, frontHeight + 1000.0f, -1000 }); // Frente, Izquierda
    int v5 = createLocalVertex({ 1000, frontHeight + 1000.0f, -1000 });  // Frente, Derecha
    int v7 = createLocalVertex({ -1000, 1000, 0 });                      // Atrás, Izquierda
    int v8 = createLocalVertex({ 1000, 1000, 0 });                       // Atrás, Derecha

    // --- TRIÁNGULOS ---
    // Cara Inferior (Base)

    TriangleCollection::createTriangle<LavaTriangle>(v1, v2, v3);
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v3, v2, v4);

    // Cara Frontal (Z = -3000)
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v4, v2, v5);
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v4, v5, v6);

    // Cara Superior (Techo)
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v6, v5, v8);
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v6, v8, v7);

    // Cara Trasera (Z = 3000)
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v1, v3, v7);
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v1, v7, v8);

    // Cara Izquierda (X = -3000)
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v3, v4, v6);
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v3, v6, v7);

    // Cara Derecha (X = 3000)
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v2, v1, v8);
    TriangleCollection::createTriangle<CheckerboardFloorTriangle>(v2, v8, v5);

    //AgentManager::createBabyAgent(*new DebugAgent2(SurfaceTransform{1, 0.25, 0.25, {0, 0, -1}}));
}