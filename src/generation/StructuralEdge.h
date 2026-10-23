//
// Created by Luis Alvarez on 16/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
#define SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
#include "Diagnosable.h"
#include "SemanticEdge.h"
#include "StructuralFace.h"
#include "Constraintsolver.h"
#include "Primitives.h"

#include <stack>
#include <utility>
#include <vector>

class StructuralFace;
struct StructuralEdge;
struct BiologicalAbomination;
struct BoundaryLoop;
class SemanticHalfEdge;

class HalfEdge {
private:
    HalfEdge(StructuralEdge& e, BoundaryLoop& s, HalfEdge& n);

  BoundaryLoop * boundaryLoop;
    HalfEdge* nextEdge; // This value must NEVER be null
    friend struct StructuralEdge;
    friend struct BiologicalAbomination;
public:
    //SemanticHalfEdge& semanticHalfEdge; // nuevo: Arista de fidelidad, se utiliza para medir tensión semántica entre dos superficies y para calcular la fidelidad (por fin la primera aparición de fidelidad en el proyecto)
    TriangleFaceType stableSurfaceType;
    StructuralEdge& structuralEdge;

    BoundaryLoop& getBoundaryLoop() const;

    HalfEdge & walk() const;

    HalfEdge & getEvilTwin() const;
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
    Length stableSize = Length(1000);
    // Probablemente añada operadores sustractivos
    ~StructuralEdge() {
        throw NonRecoverableContradictionException("StructuralEdge destructor called. This should not happen, as StructuralEdges are managed by the system and should not be deleted manually.");
    };

    friend struct BiologicalAbomination;
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
};

struct BoundaryLoop {
    StructuralFace& surface;
    HalfEdge& edge;

    [[nodiscard]] int getEdgeCount() const { // TODO temp maybe
        int count = 0;
        HalfEdge* currentEdge = &edge;
        do {
            count++;
            currentEdge = &currentEdge->walk();
        } while (currentEdge != &edge);
        return count;
    }
};

class Ribosome;

// Esta clase es responsable de crear planes de mitosis para el crecimiento con desplazamiento neto
class ARN {
    std::queue<SemanticHalfEdge*> edgesChain;
    ARN(std::queue<SemanticHalfEdge*> arn) : edgesChain(std::move(arn)) {
        if (edgesChain.empty()) {
            throw NonRecoverableContradictionException("MitosisPlan cannot be created as empty.");
        }
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

// La cantidad de operadores debe ser el mínimo posible

// Mudando a operadores b-rep Euler
// Operadores topológicos: mitosis, fold
// Operadores semánticos: heredar de otra arista, relaciones, proporciones, angulos, textura
// operadores propagadores: resuelven mediante ecuaciones de las restricciones a traves de las variable y scalar, propagando cambios a otras aristas y superficies
// Invariante: La geometría se renderiza entre cada operación, por lo que los operadores deben recibir geometría renderizable y devolver geometría renderizable.
// Invariante: la geometría es un grafo de grado >=3, cada uno de los vértices de la geometría debe estar conectado a más de 2 aristas, y cada arista debe estar conectada a exactamente 2 superficies. La estructura de datos y los operadores deben reflejarla
// La topología es una esfera, los toros están fuera del alcance del proyecto por que no tienen apariciones relevantes en la topología de Mario 64
// Nueva idea: tal vez debería hacer un corte boundary loop en la geometría semántica y garantizar su conexión topológicamente correcta mediante un contrato, ambas partes pueden ser modificadas para encajar, aún si violan la separación angular por parte de la semántica
// Se basaría en las diferencias de semántica de medias aristas para decidir hasta donde cortar, se preferirá bordear las aristas semánticas con alta diferencia semántica, mientras que se preferirá incluir medias aristas con baja diferencia
// Mientras haya fidelidad arquitectónica baja, o la habitación sea demasiado pequeña, se preferirá cortar un trozo más grande de la semántica. No estoy seguro de que vocabulario usar para describirlo.
 // Se preferirá hacer cortes cuyas medias aristas
// semánticas involucren la menor cantidad de superficies posibles (mínimo 3 para preservar la
// invariante de vértices grado >= 3)
struct BiologicalAbomination {
    static bool ranAbiogenesis;
    static std::vector<StructuralEdge*> cellBorders;
    static Scalar<ItemCount> itemCount/* = Scalar<ItemCount>()*/;
    static InfiniteTravelInvariant* infiniteTravelInvariant;

    // genera nuevas aristas estructurales desde memoria basura y devuelve una arista estructural (crea vida a partir de materia inerte)
    // TODO cubrir todas las propiedades
    static void abiogenesis(StructuralEdge*& edge);

    // La posición del nuevo edge siempre será indefinida, activará un TotallyUnrepresentableState.
    // Los structural edges de salida serán promesas que heredarán semántica, no están listos para ser usados.
    static void cellWallGrow(HalfEdge& halfEdgeA, HalfEdge& halfEdgeB, std::vector<StructuralEdge*>& outStructuralEdges, StructuralFace*& outNewSurface);
    /*  Merging this code with mitosis.
        static void fold(HalfEdge& edgeA, HalfEdge& edgeB, StructuralEdge*& outStructuralEdge,
       StructuralFace*& outStructuralFace) { StructuralFace* newSurface = new StructuralFace();
            StructuralEdge* newEdge = new StructuralEdge(edgeA.structuralEdge); // TODO complete

            if (&edgeA.surface != &edgeB.surface) {
                throw NonRecoverableContradictionException("Cannot fold edges from different
       surfaces.");
            }

            HalfEdge* h = &edgeA;

            while (h != &edgeB) {
                h->surface = newSurface;
                h = h->nextEdge;
            }
            edgeB.surface = newSurface;

            outStructuralEdge = newEdge;
            outStructuralFace = newSurface;
        }*/

    static void inheritRelation(StructuralEdge& fromEdge, StructuralEdge& toEdge);

    static void reWeight();

    static void inject(ARN& arn);

    static void grow();
};

#endif // SM64_THE_MUTANT_LIMINALITY_STRUCTURALEDGE_H
