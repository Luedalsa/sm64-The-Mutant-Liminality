//
// Created by Luis Alvarez on 18/07/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_DEBUGEXPORT_H
#define SM64_THE_MUTANT_LIMINALITY_DEBUGEXPORT_H

#include <fstream>
#include <vector>
#include "VertexCollection.h"
#include "AbstractVertex.h"
#include "TriangleCollection.h"

inline void exportTrianglesToObj(
    const std::string& path,
    const std::string& groupName = "dump"
) {
    std::ofstream out(path);
    if (!out.is_open()) return;

    std::unordered_map<int,int> remap;
    std::vector<Vector3> localVerts;

    for (int t = 0; t < VertexCollection::getVertexCount(); ++t) {
        auto v = VertexCollection::getVertex(t)->position;
        out << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }

    out << "g " << groupName << "\n";

    for (int t = 0; t < TriangleCollection::getTriangleCount(); ++t) {
        if (TriangleCollection::getTriangle(t)->isBuilt()) continue;
        auto tri = TriangleCollection::getTriangle(t);
        auto v1 = tri->getVertex(0);
        auto v2 = tri->getVertex(1);
        auto v3 = tri->getVertex(2);
        out << "f " << (v1 + 1) << " " << (v2 + 1) << " " << (v3 + 1) << "\n";
    }
}

#endif // SM64_THE_MUTANT_LIMINALITY_DEBUGEXPORT_H
