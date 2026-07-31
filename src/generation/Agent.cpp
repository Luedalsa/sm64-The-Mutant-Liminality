//
// Created by Luis Alvarez on 19/06/2026.
//

#include "Agent.h"

#include "TriangleOverlapSolver.h"

void DebugAgent2::grow() {
    energy--;

    float fy = birthPosition.forward.y;
    constexpr float EPS = 0.001f;
    float cosPitch = std::sqrt(std::max(0.0f, 1.0f - fy * fy));
    float frontHeight = (cosPitch > EPS) ? (fy * 3000.0f / cosPitch) : std::copysign(30000.0f, fy);

    std::vector<AgentTriangleRequest> prismFaces;

    constexpr int kSides = 8;
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float cx = 1500.0f, cz = -1200.0f; // Centro del octágono en XZ
    constexpr float R = 1500.0f;                 // Circunradio (mismo "ancho" que la caja original)
    constexpr float yBottom = 800.0f, yTop = 2000.0f;

    std::array<Vector3, kSides> bottom, top;
    for (int i = 0; i < kSides; ++i) {
        float angle = kPi * 2.0f * static_cast<float>(i) / static_cast<float>(kSides);
        float x = cx + R * std::cos(angle);
        float z = cz + R * std::sin(angle);
        bottom[i] = Vector3(x, yBottom, z);
        top[i]    = Vector3(x, yTop, z);
    }

    auto floorFactory = [](int a, int b, int c) {
        return TriangleCollection::createTriangle(a, b, c, TriangleFaceType::CheckerboardFloor);
    };

    // Piso (normal +y, hacia arriba, adentro del prisma): abanico desde vértice 0, orden CW.
    for (int i = 1; i < kSides - 1; ++i) {
        prismFaces.push_back({bottom[0], bottom[i + 1], bottom[i], floorFactory});
    }

    // Techo (normal -y, hacia abajo, adentro del prisma): abanico desde vértice 0, orden CCW.
    for (int i = 1; i < kSides - 1; ++i) {
        prismFaces.push_back({top[0], top[i], top[i + 1], floorFactory});
    }

    // Paredes laterales (normal hacia el centro del octágono, adentro del prisma).
    for (int i = 0; i < kSides; ++i) {
        int next = (i + 1) % kSides;
        prismFaces.push_back({bottom[i], bottom[next], top[next], floorFactory});
        prismFaces.push_back({bottom[i], top[next], top[i], floorFactory});
    }

    TriangleOverlapSolver::resolve(prismFaces);
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

    TriangleCollection::createTriangle(v1, v2, v3, TriangleFaceType::Lava);
    TriangleCollection::createTriangle(v3, v2, v4, TriangleFaceType::CheckerboardFloor);

    // Cara Frontal (Z = -3000)
    TriangleCollection::createTriangle(v4, v2, v5, TriangleFaceType::CheckerboardFloor);
    TriangleCollection::createTriangle(v4, v5, v6, TriangleFaceType::CheckerboardFloor);

    // Cara Superior (Techo)
    TriangleCollection::createTriangle(v6, v5, v8, TriangleFaceType::CheckerboardFloor);
    TriangleCollection::createTriangle(v6, v8, v7, TriangleFaceType::CheckerboardFloor);

    // Cara Trasera (Z = 3000)
    TriangleCollection::createTriangle(v1, v3, v7, TriangleFaceType::CheckerboardFloor);
    TriangleCollection::createTriangle(v1, v7, v8, TriangleFaceType::CheckerboardFloor);

    // Cara Izquierda (X = -3000)
    TriangleCollection::createTriangle(v3, v4, v6, TriangleFaceType::CheckerboardFloor);
    TriangleCollection::createTriangle(v3, v6, v7, TriangleFaceType::CheckerboardFloor);

    // Cara Derecha (X = 3000)
    TriangleCollection::createTriangle(v2, v1, v8, TriangleFaceType::CheckerboardFloor);
    TriangleCollection::createTriangle(v2, v8, v5, TriangleFaceType::CheckerboardFloor);

    AgentManager::createBabyAgent(*new DebugAgent2(SurfaceTransform{1, 0.25, 0.25, {0, 0, -1}}));
}