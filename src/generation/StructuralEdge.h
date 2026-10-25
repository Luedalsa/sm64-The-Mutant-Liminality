//
// Created by Luis Alvarez on 16/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
#define SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
#include "Diagnosable.h"
#include "SemanticEdge.h"
#include "StructuralFace.h"
#include "ConstraintSolver.h"
#include "Primitives.h"

#include <cassert>
#include <utility>

class StructuralFace;
struct StructuralEdge;
struct BoundaryLoop;
class SemanticHalfEdge;

class HalfEdge {
private:
    HalfEdge(StructuralEdge& e, BoundaryLoop& s, HalfEdge& n);
    ~HalfEdge() {
        assert(false && "HalfEdge destructor called. This should not happen, as HalfEdges are managed by the system and should not be deleted manually.");
    };

  BoundaryLoop * boundaryLoop;
    HalfEdge* nextEdge; // This value must NEVER be null
    friend struct StructuralEdge;
    friend struct ConstraintOperator;
public:
    //SemanticHalfEdge& semanticHalfEdge; // nuevo: Arista de fidelidad, se utiliza para medir tensión semántica entre dos superficies y para calcular la fidelidad (por fin la primera aparición de fidelidad en el proyecto)
    TriangleFaceType stableSurfaceType;
    StructuralEdge& structuralEdge;

    BoundaryLoop& getBoundaryLoop() const;

    HalfEdge & walk() const;

    HalfEdge & getEvilTwin() const;
    float getAngleSeparation() const;
};

// Nótese que StructuralEdge no tiene un constructor por defecto, ya que siempre debe estar asociado a dos superficies y dos edges siguientes. Esto garantiza que la estructura de la geometría sea consistente y evita la creación de edges huérfanos o mal definidos.
// Es una invariante del sistema que cada StructuralEdge esté correctamente conectado a sus superficies y edges adyacentes, tambien reduce las vulnerabilidades en la estructura de datos en runtime, no necesita asserts y reduce la carga cognitiva de mantener toda la arquitectura en mi memoria de trabajo al momento de establecer restricciones
// Los vértices no tienen estructura de datos para evitar redundancias, evitar problemas de sincronización y ahorrar memoria
// TODO debido a la nueva propiedad semanticHalfEdge, toda arista estructural deberá construirse con todas sus propiedades ya definidas
struct StructuralEdge : Diagnosable {
private:
    StructuralEdge(StructuralEdge & structuralEdge)
      : halfEdges{ HalfEdge(*this, *structuralEdge.halfEdges[0].boundaryLoop,
                            *structuralEdge.halfEdges[0].nextEdge),
                   HalfEdge(*this, *structuralEdge.halfEdges[1].boundaryLoop,
                            *structuralEdge.halfEdges[1].nextEdge) },
        //stableSize(structuralEdge.stableSize),
        displayVertex(structuralEdge.displayVertex),
        //stableSeparation(structuralEdge.stableSeparation),
        semanticTension(structuralEdge.semanticTension) {

    }
    HalfEdge halfEdges[2]; // par no ordenado, quizas crear una estructura de datos para encapsular y operar sobre ellos
    // Probablemente añada operadores sustractivos
    ~StructuralEdge() {
        assert(false && "StructuralEdge destructor called. This should not happen, as StructuralEdges are managed by the system and should not be deleted manually.");
    };

    friend struct ConstraintOperator;
public:

    // TODO: Los half edges son pares no ordenados, no se debería acceder arbitrariamente a un primero o segundo
    HalfEdge& getHalfEdge(int index) {
        if (index < 0 || index > 1) {
            throw std::out_of_range("HalfEdge index must be 0 or 1.");
        }
        return halfEdges[index];
    }
    DisplayVertex displayVertex = DisplayVertex{{0, 0, 0}, 0, 0, 0, Vector3::zero()};
    float stableSeparation = M_PI / 2.0f; // TODO: esto debería ser un Theta, pero por ahora lo dejo como float para simplificar la implementación inicial
    Length semanticTension = Length(0);

    Length stableSize = Length(1000);
    Theta theta = Theta(0);
    Theta phi = Theta(0);
};

class BoundaryLoop {
    BoundaryLoop(StructuralFace& s, HalfEdge& e) : surface(&s), edge(&e) {
    }
    StructuralFace* surface;
    HalfEdge* edge;
    friend struct ConstraintOperator;
public:
    [[nodiscard]] StructuralFace& getSurface() const {
        assert(surface != nullptr && "BoundaryLoop has no associated surface.");
        assert(edge != nullptr && "BoundaryLoop has no associated edge.");
        assert(&edge->getBoundaryLoop() == this && "BoundaryLoop's edge does not point back to this BoundaryLoop.");
        return *surface;
    }
    
    [[nodiscard]] HalfEdge& getFirstEdge() const {
        assert(edge != nullptr && "BoundaryLoop has no associated edge.");
        return *edge;
    }

    [[nodiscard]] int getEdgeCount() const { // TODO temp maybe
        int count = 0;
        HalfEdge* currentEdge = edge;
        do {
            count++;
            currentEdge = &currentEdge->walk();
        } while (currentEdge != edge);
        return count;
    }
};

class Ribosome;

// Esta clase es responsable de crear planes de mitosis para el crecimiento con desplazamiento neto
class ARN {
    std::queue<SemanticHalfEdge*> edgesChain;
    ARN(std::queue<SemanticHalfEdge*> arn) : edgesChain(std::move(arn)) {
        assert(!edgesChain.empty() && "MitosisPlan cannot be created as empty.");
    }
    friend Ribosome;
};

class Ribosome {
    static std::queue<ARN> mitosisPlans;
    //static UncompletedCellularDivisionConstraint* uncompletedDivisionConstraint;
public:

    inline static auto hasARN = Scalar<bool>();
    inline static auto printing = Scalar<bool>();

    static void addMitosisPlan(const ARN& plan) {
        mitosisPlans.push(plan);
    }
    static SemanticHalfEdge& produce() {
        if (mitosisPlans.empty()) {
            //hasARN.set(false);
            if (printing.get()) {
                std::cout << "Ribosome: No mitosis plans available, searching for new plan..." << std::endl;
                // TODO find new plan for the most unstable edge
                //mitosisPlans.push(ARN(std::queue<SemanticHalfEdge*>(baseCastle[1].semanticHalfEdges[1]))); // stub!
                //hasARN.set(true);
            }
        }
        std::queue<SemanticHalfEdge*>& edgesChain = mitosisPlans.front().edgesChain;
        SemanticHalfEdge* nextEdge = edgesChain.front();
        edgesChain.pop();

        // TODO Aqui va la lógica de impresión

        if (edgesChain.empty()) {
            mitosisPlans.pop();
        }
        return *nextEdge;
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
