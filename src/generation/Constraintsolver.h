//
// Created by Luis Alvarez on 13/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_CONSTRAINTSOLVER_H
#define SM64_THE_MUTANT_LIMINALITY_CONSTRAINTSOLVER_H

#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "Relation.h"
#include "SemanticEdge.h"

using Edge = SemanticEdge;

class ConstraintSolver {
public:
    RelationId addRelation(Relation r);
    int32_t    addEdge(SemanticEdge e);

    Relation& relation(RelationId id);
    SemanticEdge&     edge(int32_t id);

    // ---- detección --------------------------------------------------------

    // ¿Existe una contraparte simétrica válida para este edge?
    // (stub simple: compara si counterpartEdgeId existe y su |orientación|
    // coincide en espejo; en tu motor real esto miraría el árbol semántico)
    bool checkSymmetry(int32_t edgeId, int32_t counterpartEdgeId) const;

    // ¿Dos edges están lo bastante cerca Y comparten semántica (relation) como
    // para fusionarse?
    bool checkProximityMerge(int32_t a, int32_t b, float maxDistance) const;

    // ¿El tamaño actual del edge se sale de la tolerancia de su relación?
    bool isToleranceExceeded(int32_t edgeId) const;

    // ---- resolución ---------------------------------------------------

    // Fusiona `absorbed` dentro de `survivor` (arista 16 -> arista 17).
    void mergeEdges(int32_t survivor, int32_t absorbed);

    // El edge no puede rotar (orientationLocked): en vez de eso, absorbe el
    // ajuste requerido como incremento/decremento de tamaño.
    void lockOrientationGrow(int32_t edgeId, float requiredDelta);

    // Recalcula min/max (DerivedMinMax) para una relación repetida, mirando
    // los edges actualmente enlazados a su unidad repetida.
    void resolveDerivedTolerance(RelationId repeatedRelationId);

    // Si `id` superó su tolerancia, avisa a las relaciones compuestas padre
    // para que rebalanceen sus invariantes (propagación hacia arriba).
    void propagateConstraint(RelationId id);

    // Aplica la invariante SumPreserving entre los componentes directos de
    // una relación compuesta (relación 10 = {8, 9}).
    void enforceSumInvariant(RelationId compositeId);

    // Registers a compiled semantic map and resolves all constraints until
    // no further propagation is required.
    bool solve(std::vector<SemanticEdge>& edges);

    void log(const std::string& tag, const std::string& msg) const;

private:
    std::vector<RelationId> parentsOf(RelationId child) const;
    int32_t representativeEdgeFor(RelationId r) const; // primer edge vivo enlazado

    std::unordered_map<RelationId, Relation> relations_;
    std::unordered_map<int32_t, SemanticEdge> edges_;
    std::unordered_multimap<RelationId, int32_t> edgesByRelation_;
};

#endif // SM64_THE_MUTANT_LIMINALITY_CONSTRAINTSOLVER_H
