//
// Created by Luis Alvarez on 19/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_DIAGNOSABLE_H
#define SM64_THE_MUTANT_LIMINALITY_DIAGNOSABLE_H
// #include "Constraintsolver.h"

#include <functional>
#include <queue>
#include <string>
#include <vector>

enum class Diagnostic {
    ANGLE_VIOLATION,
    SEMANTIC_TENSION,
    TOO_LOW_SYMMETRY,
    NOT_SATISFIED_EDGE_SURFACES,
    NOT_SATISFIED_SURFACES_EDGES,
};

class Constraint;

struct Diagnosable {
    static std::vector<Diagnostic> diagnostics;
    static std::priority_queue<Constraint*, std::vector<Constraint*>, std::function<bool(Constraint *, Constraint *)>> constraints;
public:
    static void addDiagnostic(Diagnostic diagnostic) {
        diagnostics.emplace_back(diagnostic);
    }
    static void addViolatedConstraint(Constraint* constraint) {
        constraints.push(constraint);
    }
    static const std::priority_queue<Constraint*, std::vector<Constraint*>, std::function<bool(Constraint *, Constraint *)>>& getConstraints() {
        return constraints;
    }
    static const std::vector<Diagnostic>& getDiagnostics() {
        return diagnostics;
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_DIAGNOSABLE_H
