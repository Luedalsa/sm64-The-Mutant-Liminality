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
#include "NonRecoverableContradictionException.h"
#include "StagedBatch.h"

#include <cassert>
#include <optional>
#include <utility>
#include <cmath>

class StructuralFace;
struct StructuralEdge;
struct BoundaryLoop;
class SemanticHalfEdge;

class HalfEdge {
private:
    HalfEdge(StructuralEdge& e);
    ~HalfEdge() {
        assert(false && "HalfEdge destructor called. This should not happen, as HalfEdges are managed by the system and should not be deleted manually.");
    };

    // Links boundaryLoop exactly once, after construction. Only ConstraintOperator can call it.
    void linkBoundaryLoop(BoundaryLoop& loop);

    BoundaryLoop* boundaryLoop = nullptr; // Immutable once linked
    // Must NEVER be null once the HalfEdge has been linked. It is the only link that can be
    // reassigned, because ConstraintOperator::cellWallGrow splices half edge rings.
    HalfEdge* nextEdge = nullptr;
    // True once every link this HalfEdge needs is set. Queried by StagedBatch.
    [[nodiscard]] bool isLinked() const { return boundaryLoop != nullptr && nextEdge != nullptr; }
    friend struct StructuralEdge;
    friend struct ConstraintOperator;
    template <Stageable... Ts> friend class StagedBatch;
public:
    //SemanticHalfEdge& semanticHalfEdge; // nuevo: Arista de fidelidad, se utiliza para medir tensión semántica entre dos superficies y para calcular la fidelidad (por fin la primera aparición de fidelidad en el proyecto)
    TriangleFaceType stableSurfaceType = TriangleFaceType::CheckerboardFloor;
    StructuralEdge& structuralEdge;

    BoundaryLoop& getBoundaryLoop() const;

    HalfEdge & walk() const;

    HalfEdge & getEvilTwin() const;
    float getAngleSeparation() const;
};

// Nótese que StructuralEdge solo puede ser construida por ConstraintOperator, sin recibir otra StructuralEdge: sus medias aristas nacen sin enlazar (punteros nulos) y ConstraintOperator las conecta después a sus superficies y edges adyacentes.
// Es una invariante del sistema que cada StructuralEdge esté correctamente conectado a sus superficies y edges adyacentes una vez termina la operación que la creó
// Los vértices no tienen estructura de datos para evitar redundancias, evitar problemas de sincronización y ahorrar memoria
// TODO debido a la nueva propiedad semanticHalfEdge, toda arista estructural deberá construirse con todas sus propiedades ya definidas
struct StructuralEdge : Diagnosable {
private:
    // No recibe otra StructuralEdge: todas sus propiedades se inicializan con valores por defecto o punteros nulos.
    StructuralEdge();
    HalfEdge halfEdges[2]; // par no ordenado, quizas crear una estructura de datos para encapsular y operar sobre ellos
    // Probablemente añada operadores sustractivos
    ~StructuralEdge();
    // Las constraints no existen hasta la etapa de restricciones (StructuralEdge::constrain), cuando los enlaces ya están completos.
    // Son optional porque no son copiables ni movibles (se registran como observadores en su constructor).
    std::optional<NormalSeparationConstraint> normalSeparationConstraint;
    std::optional<DirectionVerticesRelationSizeConstraint> directionVerticesRelationSizeConstraint;

    // True once both HalfEdges are fully linked. Queried by StagedBatch.
    [[nodiscard]] bool isLinked() const;
    // Creates the constraints exactly once, after every participant of the batch is linked. Called by StagedBatch.
    void constrain();

    friend struct ConstraintOperator;
    template <Stageable... Ts> friend class StagedBatch;
public:

    // TODO: Los half edges son pares no ordenados, no se debería acceder arbitrariamente a un primero o segundo
    HalfEdge& getHalfEdge(int index);
    DisplayVertex displayVertex = DisplayVertex{{0, 0, 0}, 0, 0, 0, Vector3::zero()};
    Theta stableSeparation = std::numbers::pi / 2.0f;
    Length semanticTension = Length(0);

    Length stableSize = Length(1000);
    Theta sphericalDirection[2] = {Theta(0), Theta(0)};
};

class BoundaryLoop {
    BoundaryLoop() = default;
    StructuralFace* surface = nullptr;
    HalfEdge* edge = nullptr;

    // Links surface and edge exactly once, after construction. Only ConstraintOperator can call it.
    void link(StructuralFace& s, HalfEdge& e) {
        assert(surface == nullptr && edge == nullptr && "BoundaryLoop is already linked.");
        if (surface != nullptr || edge != nullptr) {
            throw NonRecoverableContradictionException("BoundaryLoop is already linked and its links are immutable.");
        }
        surface = &s;
        edge = &e;
    }
    // True once surface and edge are both set. Queried by StagedBatch.
    [[nodiscard]] bool isLinked() const { return surface != nullptr && edge != nullptr; }
    friend struct ConstraintOperator;
    template <Stageable... Ts> friend class StagedBatch;
public:
    [[nodiscard]] StructuralFace& getSurface() const {
        assert(surface != nullptr && "BoundaryLoop has no associated surface.");
        if (surface == nullptr) {
            throw NonRecoverableContradictionException("BoundaryLoop has no associated surface.");
        }
        assert(&getFirstEdge().getBoundaryLoop() == this && "BoundaryLoop's edge does not point back to this BoundaryLoop.");
        return *surface;
    }
    
    [[nodiscard]] HalfEdge& getFirstEdge() const {
        assert(edge != nullptr && "BoundaryLoop has no associated edge.");
        if (edge == nullptr) {
            throw NonRecoverableContradictionException("BoundaryLoop has no associated edge.");
        }
        return *edge;
    }

    [[nodiscard]] int getEdgeCount() const { // TODO temp maybe
        int count = 0;
        HalfEdge* firstEdge = &getFirstEdge();
        HalfEdge* currentEdge = firstEdge;
        do {
            count++;
            currentEdge = &currentEdge->walk();
        } while (currentEdge != firstEdge);
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
