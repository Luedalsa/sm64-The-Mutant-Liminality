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
#include "NonRecoverableContradictionException.h"

#include <string>
#include <unordered_map>
#include <vector>
#include "Relation.h"
#include "SemanticEdge.h"

struct NormalSeparationConstraint : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::TotallyUnrepresentable;
    }
    const DirectionAndSeparation& edgeDirectionAndSeparation;
    SphericalDirection& normalA;
    SphericalDirection& normalB;

    NormalSeparationConstraint(const DirectionAndSeparation& edgeDirectionAndSeparation, SphericalDirection& normalA, SphericalDirection& normalB)
        : edgeDirectionAndSeparation(edgeDirectionAndSeparation), normalA(normalA), normalB(normalB) {}

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
struct NetDisplacementConstraint : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::GeometricallyUnrepresentable;
    }
    std::vector<Length*>& lengthsVector;

    NetDisplacementConstraint(std::vector<Length*>& lengthsVector) : lengthsVector(lengthsVector) {}

    SolutionStatus solve() override {
        // find lowest priority proportional relation
        // adapt size of lowest priority proportional relation to sum of all lengths or mitosis
        float sum = 0.0f;
        for (const auto& length : lengthsVector) {
            sum += length->value;
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

struct InfiniteTravelInvariant : public Constraint {
    const ConstraintViolationSeverity getSeverity() const override {
        return ConstraintViolationSeverity::FullyRenderable;
    }
    ItemCount& structuralEdgeCount;

    InfiniteTravelInvariant(ItemCount& structuralEdgeCount) : structuralEdgeCount(structuralEdgeCount) {}

    SolutionStatus solve() override {
        if (structuralEdgeCount.get() == INT_MAX) {
            return SolutionStatus::SUCCESS; // unreachable
        } else {
            // check less stable/priority edge
            // mitosis
        }
        return SolutionStatus::POSTPONED;
    }
};*/

class ConstraintSolver {
public:
    static int currentFrame;

    static void checkAndSolve() {
        currentFrame++;
        while (true) { // TODO while there are totally unrepresentable states do
            if (Diagnosable::getConstraints().empty()) {
                throw NonRecoverableContradictionException("ConstraintSolver: No constraints to solve, Generator reached a deadlock so it can't generate more geometry, violating invariant of infinite travel.");
            }
            auto constraint = Diagnosable::getConstraints()[0];
            if (constraint->state == ConstraintState::VIOLATED) {
                SolutionStatus status = constraint->solve();
                if (status == SolutionStatus::SUCCESS) {
                    constraint->state = ConstraintState::SATISFIED;
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
                    throw NonRecoverableContradictionException("Reached contradiction, multiple constraints are in conflict and cannot be satisfied simultaneously. The system is in an unrecoverable state.");
                }
                constraint->prevStatus = status;
            } else {
                throw NonRecoverableContradictionException("ConstraintSolver: Constraint is not in CONTRADICTION state when checking constraints. Diagnoser didnt do a good job.");
            }
        }
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_CONSTRAINTSOLVER_H
