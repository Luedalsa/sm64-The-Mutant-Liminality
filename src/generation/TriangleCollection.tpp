#pragma once
#include "VertexCollection.h"
#include "AbstractTriangle.h"

template <class T, class... Args>
int TriangleCollection::createTriangle(Args&&... args) {
    std::unique_ptr<AbstractTriangle> tri(new T(std::forward<Args>(args)...));
    auto v1 = tri->getVertex(0);
    auto v2 = tri->getVertex(1);
    auto v3 = tri->getVertex(2);
    triangles.push_back(std::move(tri));
    int id = triangles.size() - 1;
    triangles[id]->selfIndex = id;
    if (VertexCollection::getVertexCount() > 3 && VertexCollection::getVertex(v1)->connectingTriangles.empty() && VertexCollection::getVertex(v2)->connectingTriangles.empty() && VertexCollection::getVertex(v3)->connectingTriangles.empty()) {
        std::cerr << "Warning: Triangle " << id << " is disconnected from the collection. Breadth-first search (BFS) is expected to fail\n";
        VertexCollection::getVertex(0)->connectingTriangles.push_back(id);
        }
    VertexCollection::getVertex(v1)->connectingTriangles.push_back(id);
    VertexCollection::getVertex(v2)->connectingTriangles.push_back(id);
    VertexCollection::getVertex(v3)->connectingTriangles.push_back(id);
    return id;
}