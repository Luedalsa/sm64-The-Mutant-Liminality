//
// Created by Luis Alvarez on 19/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_DIAGNOSABLE_H
#define SM64_THE_MUTANT_LIMINALITY_DIAGNOSABLE_H
#include <string>
#include <vector>

enum class Diagnostic {
    ANGLE_VIOLATION,
    SEMANTIC_TENSION,
    TOO_LOW_SYMMETRY,
    NOT_SATISFIED_EDGE_SURFACES,
    NOT_SATISFIED_SURFACES_EDGES,
};

struct Diagnosable {
std::vector<Diagnostic> diagnostics;
public:
virtual ~Diagnosable() = default;
    virtual void diagnose() = 0;
    void addDiagnostic(Diagnostic diagnostic) {
        diagnostics.emplace_back(diagnostic);
    }
    const std::vector<Diagnostic>& getDiagnostics() const {
        return diagnostics;
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_DIAGNOSABLE_H
