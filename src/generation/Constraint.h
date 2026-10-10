//
// Created by Luis Alvarez on 10/10/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_CONSTRAINT_H
#define SM64_THE_MUTANT_LIMINALITY_CONSTRAINT_H
#include "ConstraintState.h"
#include "ConstraintViolationSeverity.h"
#include "SolutionStatus.h"

class Constraint {
public:
    ConstraintState state = ConstraintState::SATISFIED;
    SolutionStatus prevStatus = SolutionStatus::SUCCESS;
    virtual const ConstraintViolationSeverity getSeverity() const = 0;
    virtual SolutionStatus solve() = 0;
    virtual ~Constraint() = default;
};

#endif // SM64_THE_MUTANT_LIMINALITY_CONSTRAINT_H
