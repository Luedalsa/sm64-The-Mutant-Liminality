//
// Created by Luis Alvarez on 30/07/2026.
//
#include "TriangleTypes.h"
#include "TriangleCollection.h"
#include "AbstractTriangle.h"
#include "VertexCollection.h"
#include "AbstractVertex.h"
#include "CollisionManager.h"
#include "DisplayListManager.h"
#include "textures.h"
#include "surface_terrains.h"

#include <map>
#include <cstdlib>

std::array<BuildGeometryFn, static_cast<size_t>(TriangleFaceType::Count)> TriangleTypeRegistry::table;

namespace {

void buildFlatGeometry(std::vector<int>& neighbours, int surfaceType, const u8* texture) {
    std::map<int, int> localVertexSegment;
    std::vector<std::array<int, 3>> localTriangleSegment;
    std::vector<DisplayVertex> displayVertices;
    int currentVertex = 0;

    for (auto n : neighbours) {
        auto tri = TriangleCollection::getTriangle(n);
        AbstractVertex* vertexa = VertexCollection::getVertex(tri->getVertex(0));
        AbstractVertex* vertexb = VertexCollection::getVertex(tri->getVertex(1));
        AbstractVertex* vertexc = VertexCollection::getVertex(tri->getVertex(2));
        int vv1 = vertexa->getCollisionVertex();
        int vv2 = vertexb->getCollisionVertex();
        int vv3 = vertexc->getCollisionVertex();
        CollisionManager::addCollisionTriangle(vv1, vv2, vv3, surfaceType);

        for (int i = 0; i < 3; ++i) {
            int v = tri->getVertex(i);
            if (localVertexSegment.find(v) == localVertexSegment.end()) {
                localVertexSegment[v] = currentVertex++;
                displayVertices.push_back({
                    VertexCollection::getVertex(v)->position, 0,
                    VertexCollection::getVertex(v)->position.x,
                    VertexCollection::getVertex(v)->position.z,
                    { static_cast<float>(std::rand()), static_cast<float>(std::rand()), static_cast<float>(std::rand())}
                });
            }
        }
        localTriangleSegment.push_back({
            localVertexSegment[tri->getVertex(0)],
            localVertexSegment[tri->getVertex(1)],
            localVertexSegment[tri->getVertex(2)]
        });
    }

    DisplayListManager::addDisplayListSegment(displayVertices, localTriangleSegment, texture);
}

} // namespace

void registerBuiltinTriangleTypes() {
    TriangleTypeRegistry::registerType(TriangleFaceType::CheckerboardFloor,
        [](std::vector<int>& neighbours) { buildFlatGeometry(neighbours, SURFACE_DEFAULT, inside_09004000); });

    TriangleTypeRegistry::registerType(TriangleFaceType::Lava,
        [](std::vector<int>& neighbours) { buildFlatGeometry(neighbours, SURFACE_BURNING, fire_09009000); });

TriangleTypeRegistry::registerType(TriangleFaceType::InsideWallFirstFloor,
    [](std::vector<int>& neighbours) {
        buildFlatGeometry(neighbours, SURFACE_DEFAULT, inside_09001000);
    });
}