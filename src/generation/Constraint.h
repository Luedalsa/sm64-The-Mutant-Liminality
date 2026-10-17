//
// Created by Luis Alvarez on 10/10/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_CONSTRAINT_H
#define SM64_THE_MUTANT_LIMINALITY_CONSTRAINT_H
#include "ConstraintState.h"
#include "ConstraintViolationSeverity.h"
#include "SolutionStatus.h"

// Cualquier restricción que se pueda aplicar a un conjunto de dependientes, y además necesite ser multidireccional heredará esta estructura de datos. La restricción puede ser de cualquier tipo, pero debe poder calcular su valor y resolverlo. El sistema de resolución de restricciones se encargará de aplicar la restricción a los dependientes involucrados, y de actualizar su estado según corresponda.
class Constraint {
    virtual float calculateConstraint() = 0;
public:
    ConstraintState state = ConstraintState::SATISFIED;
    SolutionStatus prevStatus = SolutionStatus::SUCCESS;
    virtual const ConstraintViolationSeverity getSeverity() const = 0;
    virtual SolutionStatus solve() = 0;
    virtual ~Constraint() = default;

    bool isIdentity() {
        return calculateConstraint() == 0.0f;
    }

    virtual float getConstraintTension() = 0; // priority queue determinant

    bool operator >(Constraint& other) {
        if (getSeverity() == ConstraintViolationSeverity::TotallyUnrepresentable) {
            return true;
        }
        return ((getConstraintTension() * static_cast<float>(getSeverity())) > (other.getConstraintTension() * static_cast<float>(other.getSeverity())));
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_CONSTRAINT_H
