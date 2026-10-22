//
// Created by Luis Alvarez on 20/08/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_SEMANTICEDGE_H
#define SM64_THE_MUTANT_LIMINALITY_SEMANTICEDGE_H
#include "AbstractTriangle.h"
#include "BaseCastle.h"
#include "BaseEdge.h"
#include "EdgeRelations.h"
#include "StructuralEdge.h"

#include <vector>

// Conceptualmente equivalente al mapa semántico del diario per implícito, TODO: para medir tensión semántica entre dos superficies se debe hacer un pathfinding entre las nedias aristas semánticas y registrar la más alta diferencia semántica que se encuentre en el camino.
enum class SemanticDifference {
    Contextual, // Equivalente a Partición contextual según el diario
    High,
    Medium, // Equivalente a Partición local según el diario
    Low,
    Inseparable, // Equivalente a Partición estructural según el diario
};

class SemanticHalfEdge {
    SemanticHalfEdge(SemanticEdge& e, /*BoundaryLoop& s, */SemanticHalfEdge& n)
        : /*boundaryLoop(&s), */ nextEdge(&n), surfaceType(TriangleFaceType::CheckerboardFloor), semanticEdge(e) {
    }

    // BoundaryLoop* boundaryLoop;
    SemanticHalfEdge* nextEdge; // This value must NEVER be null
    //SemanticDifference semanticDifference; // Mide que tan inseparables son los enlaces entre dos medias aristas semánticas en la misma superficie.
    // probablemente remplace esta propiedad por una restricción de libertad en base a la cantidad de superficies adyacentes
    TriangleFaceType surfaceType;
    SemanticEdge& semanticEdge;

    friend class AgentManager;
public:
    SemanticHalfEdge& walk() const {
        return *nextEdge;
    }
};

// Una arista semántica es una arista que además de su geometría, tiene información semántica sobre los tipos de caras que conecta y las relaciones geométricas que debe cumplir. Esta información se utiliza para guiar la generación de geometría y asegurar que las aristas cumplan con las restricciones de diseño y estilo del nivel.
// Se usa como guía para resolver tensiones de fidelidad arquitectónica
class SemanticEdge {
    SemanticEdge(float as) : angularSeparation(as) {
    }
    friend struct BaseCastle;
  public:
    SemanticDifference semanticDifference = SemanticDifference::Low; // Mide la diferencia de semántica entre dos medias aristas semánticas, pared y pilar tienen alta diferencia semántica mientras que pared y pared baja.
    float angularSeparation;
};

#endif // SM64_THE_MUTANT_LIMINALITY_SEMANTICEDGE_H