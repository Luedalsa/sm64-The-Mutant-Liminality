//
// Created by Luis Alvarez on 13/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_CONSTRAINTSOLVER_H
#define SM64_THE_MUTANT_LIMINALITY_CONSTRAINTSOLVER_H

#pragma once
#include "Constraint.h"
#include "ConstraintState.h"
#include "Dependent.h"
#include "Diagnosable.h"

#include <cassert>
#include <string>

struct NormalSeparationConstraint : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::TotallyUnrepresentable;
    }
    const DirectionAndSeparation& edgeDirectionAndSeparation;
    SphericalDirection& normalA;
    SphericalDirection& normalB;

    NormalSeparationConstraint(const DirectionAndSeparation& edgeDirectionAndSeparation, SphericalDirection& normalA, SphericalDirection& normalB)
        : edgeDirectionAndSeparation(edgeDirectionAndSeparation), normalA(normalA), normalB(normalB) {}

    float calculateConstraint() override {/*
        // Calculate the angular separation between normalA and normalB
        float angularSeparation = std::acos(std::sin(normalA.get().theta) * std::sin(normalB.get().theta) +
                                            std::cos(normalA.get().theta) * std::cos(normalB.get().theta) *
                                            std::cos(normalA.get().phi - normalB.get().phi));
        // Compare it with the required separation from edgeDirectionAndSeparation
        float requiredSeparation = edgeDirectionAndSeparation.get().angularSeparation;
        return angularSeparation - requiredSeparation; // Positive if violated, negative if satisfied*/
    }

    float getConstraintTension() override {
        //return std::max(normalA.getPriority(), normalB.getPriority());
    }

    SolutionStatus solve() override {/*
        if (normalA.getPriority() == normalB.getPriority()) {
            return SolutionStatus::POSTPONED;
        }
        if (normalA.getPriority() > normalB.getPriority()) {
            // normalb adapts to fixed separation and normalA
        } else if (normalB.getPriority() > normalA.getPriority()) {
            // do fancy calc
        }
        if (normalA.getPriority() > edgeDirectionAndSeparation.getPriority() || normalB.getPriority() > edgeDirectionAndSeparation.getPriority()) {
            throw std::runtime_error("Angular separation priority is lower than one of the normals, this violates angular invariant");
        }*/
        return SolutionStatus::SUCCESS;
    }
};

struct DirectionVerticesRelationSizeConstraint : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::TotallyUnrepresentable;
    }
    SphericalCoordinates& edgeDirection;
    VVector3& edgeVertexA;
    VVector3& edgeVertexB;

    DirectionVerticesRelationSizeConstraint(SphericalCoordinates& edgeDirection, VVector3& edgeVertexA, VVector3& edgeVertexB)
        : edgeDirection(edgeDirection), edgeVertexA(edgeVertexA), edgeVertexB(edgeVertexB) {}

    SolutionStatus solve() override {
        // direction lowest priority: calc line that intersects two vertices
        // vertices lowest priority: calc point described by polar coordinates and other fixed vertex
        return SolutionStatus::SUCCESS;
    }
};
/*
struct NetDisplacementConstraint : public Constraint { Maybe not needed, with addition of ARN data structure
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::GeometricallyUnrepresentable;
    }
    std::vector<Length*>& lengthsVector;

    NetDisplacementConstraint(std::vector<Length*>& lengthsVector) : lengthsVector(lengthsVector) {}

    SolutionStatus solve() override {
        // find lowest priority proportional relation
        // adapt size of lowest priority proportional relation to sum of all lengths or mitosis
        float sum = 0.0f;
        Length* l = nullptr;
        for (const auto& length : lengthsVector) {
            sum += length->value;
            if (length->getPriority() < l->getPriority()) {
                l = length;
            }
        }
        // Trigger geometric contradiction alarm / stall
        return SolutionStatus::SUCCESS;
    }
};

struct RelationSemanticLengthConstraint : public Constraint { // rethink
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::SemanticallyUnrepresentable;
    }
    Variable<SemanticEdge*>& semanticEdgeLength;
    Variable<float>& edgeLengthOverload;
    Variable<ProportionalRelation*>& relationLength;

    RelationSemanticLengthConstraint(const std::pair<Variable<SemanticEdge*>&, Variable<SemanticEdge*>&> &semanticEdges, Variable<float>& edgeLengthOverload, Variable<ProportionalRelation*>& relationLength) : semanticEdgeLength(semanticEdges.first), edgeLengthOverload(edgeLengthOverload), relationLength(relationLength) {}

    SolutionStatus solve() override {
        return SolutionStatus::SUCCESS;
    }
};

struct TypesTensionConstraint : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::SemanticallyUnrepresentable;
    }
    std::vector<EnumIndex*>& surfaceFaceTypes;
    EnumIndex& sharedType;

    TypesTensionConstraint(std::vector<EnumIndex*>& surfaceFaceTypes, EnumIndex& sharedType) : surfaceFaceTypes(surfaceFaceTypes), sharedType(sharedType) {}

    SolutionStatus solve() override {
        // mitosis
        return SolutionStatus::SUCCESS;
    }
};

struct ProportionalRelationUsesConstraint : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::FullyRenderable;
    }
    std::vector<EnumIndex*>& proportionalRelations;
    ItemCount& usesCount;

    ProportionalRelationUsesConstraint(std::vector<EnumIndex*>& proportionalRelations, ItemCount& usesCount) : proportionalRelations(proportionalRelations), usesCount(usesCount) {}

    SolutionStatus solve() override {
        // find structural edge with closest size of relation
        // change edge's relation to this
        usesCount.set(static_cast<int>(proportionalRelations.size()));
        return SolutionStatus::SUCCESS;
    }
};

struct ProportionalRelationForceConstraint : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::FullyRenderable;
    }
    ItemCount& usesCount;
    ItemCount& edgesThatMustBelongToTheRelation;

    ProportionalRelationForceConstraint(ItemCount& usesCount, ItemCount& edgesThatMustBelongToTheRelation) : usesCount(usesCount), edgesThatMustBelongToTheRelation(edgesThatMustBelongToTheRelation) {}

    SolutionStatus solve() override {
        usesCount.set(edgesThatMustBelongToTheRelation.get());
        return SolutionStatus::SUCCESS;
    }
};

struct SymmetryRelationConstraint : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::FullyRenderable;
    }
    std::vector<EnumIndex*>& proportionalRelations;
    ItemCount& edgesBelongingToSymmetry;

    SymmetryRelationConstraint(std::vector<EnumIndex*>& proportionalRelations, ItemCount& edgesBelongingToSymmetry) : proportionalRelations(proportionalRelations), edgesBelongingToSymmetry(edgesBelongingToSymmetry) {}

    SolutionStatus solve() override {
        return SolutionStatus::SUCCESS;
    }
};

// Invariante de Conservación de la estructura simétrica: La geometría generada debe conservar los patrones de simetría presentes en la estructura topológica y semántica, permitiendo que las desviaciones respecto de dichos patrones ocurran únicamente dentro de los grados de libertad establecidos por las métricas del generador, no usar simetría implica negar los patrones estructurales de los castillos tanto en el juego original como en los mods.
struct SymmetryForceConstraint : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::FullyRenderable;
    }
    ItemCount& edgesBelongingToSymmetry;
    ItemCount& edgesThatMustBelongToTheSymmetry;

    SymmetryForceConstraint(ItemCount& edgesBelongingToSymmetry, ItemCount& edgesThatMustBelongToTheSymmetry) : edgesBelongingToSymmetry(edgesBelongingToSymmetry), edgesThatMustBelongToTheSymmetry(edgesThatMustBelongToTheSymmetry) {}

    SolutionStatus solve() override {
        // check relations by bfs around edge
        // find first asymmetry
        // copy semantic properties from higher priority edge to lower priority edge with flipped angle
        edgesBelongingToSymmetry.set(edgesThatMustBelongToTheSymmetry.get());
        return SolutionStatus::SUCCESS;
    }
};


struct FidelityForceConstraint : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::FullyRenderable;
    }
    ItemCount& edgesBelongingLocalPartition;
    ItemCount& edgesThatMustBelong;
    FidelityForceConstraint(ItemCount& edgesBelongingLocalPartition, ItemCount& edgesThatMustBelong) : edgesBelongingLocalPartition(edgesBelongingLocalPartition), edgesThatMustBelong(edgesThatMustBelong) {}


    SolutionStatus solve() override {
        // check structural edges by bfs around edge
        // find first infidelity
        // copy semantic properties from higher priority edge to lower priority edge or mitosis
        edgesBelongingLocalPartition.set(edgesThatMustBelong.get());
        return SolutionStatus::SUCCESS;
    }
};
*/

// Invariante de Recorrido infinito: Siempre hay algo que generar en alguna parte, este es el centro de todos los pivotes del proyecto.
struct InfiniteTravelInvariant : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::FullyRenderable;
    }
    ItemCount& structuralEdgeCount;

    InfiniteTravelInvariant(ItemCount& structuralEdgeCount) : structuralEdgeCount(structuralEdgeCount) {
        structuralEdgeCount.addObserver(this);
    }

    float calculateConstraint() override {
        return 0.01f;
    }

    float getConstraintTension() override {
        return 0.01f;
    }

    SolutionStatus solve() override {
        if (structuralEdgeCount.get() == INT_MAX) {
            return SolutionStatus::SUCCESS; // unreachable
        } else {
            //Ribosome::printing.set(true);
        }
        return SolutionStatus::PROGRESS;
    }
};

// TODO: Necesito que algo tenga la responsabilidad de trackear los procesos de mitosis para asegurarse de que la superficie que van copiando terminen con un half edge paralelo o con una suma angular neta de 0°
struct NetAngleBundaryConstraint : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::TotallyUnrepresentable;
    }

    SolutionStatus solve() override {
    }
};

struct UncompletedCellularDivisionConstraint : public Constraint {
    Scalar<bool>& printing;

    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::GeometricallyUnrepresentable;
    }

    UncompletedCellularDivisionConstraint(Scalar<bool>& printing) : printing(printing) {}

    float calculateConstraint() override {
        return printing.get() ? 1.0f : 0.0f; // If printing is true, the constraint is violated (1.0), otherwise it's satisfied (0.0)
    }

    float getConstraintTension() override {
        return printing.getPriority(); // The tension is based on the priority of the printing variable
    }

    SolutionStatus solve() override {
        //BiologicalAbomination::grow();
        if (printing.get()) {
            return SolutionStatus::PROGRESS;
        }
        return SolutionStatus::SUCCESS;
    }
};

struct RibosomeHasARNConstraint : public Constraint {
    Scalar<bool>& printing;
    Scalar<bool>& hasARN;

    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::FullyRenderable;
    }

    RibosomeHasARNConstraint(Scalar<bool>& printing, Scalar<bool>& hasARN) : printing(printing), hasARN(hasARN) {}

    float calculateConstraint() override {
        return hasARN.get() ? !printing.get() : 0.0f;
    }

    float getConstraintTension() override {
        return printing.getPriority();
    }

    SolutionStatus solve() override {
        //printing.set(true);
        return SolutionStatus::SUCCESS;
    }
};

struct SemanticDiversityConstraint : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::FullyRenderable;
    }

    SemanticDiversityConstraint() {}

    float calculateConstraint() override {
    }

    float getConstraintTension() override {
    }

    SolutionStatus solve() override {
        return SolutionStatus::POSTPONED;
    }
};

struct SurfaceAdjacencyQuantityConstraint : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::FullyRenderable;
    }

    SurfaceAdjacencyQuantityConstraint() {}

    float calculateConstraint() override {
    }

    float getConstraintTension() override {
    }

    SolutionStatus solve() override {
        return SolutionStatus::POSTPONED;
    }
};

// Esta restricción es un poco más compleja que las otras
// Mide que tanto una media arista tiene tensión semántica con otra
// Para cada arista estructural, sus dos medias aristas estructurales deben tener la misma tensión semántica que sus dos medias aristas semánticas
struct SemanticVoltageConstraint : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::FullyRenderable;
    }

    SemanticVoltageConstraint() {}

    float calculateConstraint() override {
    }

    float getConstraintTension() override {
    }

    SolutionStatus solve() override {
        return SolutionStatus::POSTPONED;
    }
};

// TODO añadir:
// Invariante de Localidad geométrica: Dado que es extremadamente dificil hacer geometría bonita, variada y teselable, cada sección visible tiene su propio espacio euclideano, si es bonita y variada, no es teselable; si es bonita y teselable, no es variada; si es variada y teselable, no es bonita. En este caso priorizo que la geometría sea bonita y variada.
class ConstraintSolver {
public:
    static int currentFrame;

    static void checkAndSolve() {
        currentFrame++;
        bool continueSolving = true;
        while (continueSolving) {
            assert(!Diagnosable::getViolatedConstraints().empty() && "ConstraintSolver: No constraints to solve, Generator reached a deadlock so it can't generate more geometry, violating invariant of infinite travel.");
            auto constraint = Diagnosable::getViolatedConstraints().top();
            if (constraint->state == ConstraintState::VIOLATED) {
                SolutionStatus status = constraint->solve();
                if (status == SolutionStatus::SUCCESS) {
                    constraint->state = ConstraintState::SATISFIED;
                    assert(constraint->isIdentity() && "ConstraintSolver: Constraint is not in identity state after success solving.");
                    //Diagnosable::getConstraints().erase(Diagnosable::constraints.begin());
                    /*if (constraint->severity == ConstraintViolationSeverity::GeometricallyUnrepresentable) {
                    }*/
                } else if (status == SolutionStatus::POSTPONED) {
                    /*if (constraint.prevStatus == SolutionStatus::POSTPONED) {
                        throw NonRecoverableContradictionException("ConstraintSolver: Constraint is in POSTPONED state for two consecutive frames. This indicates a deadlock or circular dependency in the constraints, which cannot be resolved.");
                    }*/
                    //Diagnosable::getConstraints().erase(Diagnosable::constraints.begin());
                    //Diagnosable::getConstraints().push_back(constraint);
                } else if (status == SolutionStatus::CONTRADICTION) {
                    assert(false && "Reached contradiction, multiple constraints are in conflict and cannot be satisfied simultaneously. The system is in an unrecoverable state.");
                }
                constraint->prevStatus = status;
            } else {
                assert(false && "ConstraintSolver: Constraint is not in CONTRADICTION state when checking constraints. Diagnoser didnt do a good job.");
            }
            continueSolving = Diagnosable::getViolatedConstraints().top()->getSeverity() == ConstraintViolationSeverity::TotallyUnrepresentable;
        }
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_CONSTRAINTSOLVER_H
