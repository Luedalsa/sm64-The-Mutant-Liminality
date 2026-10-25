//
// Created by Luis Alvarez on 29/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_DEPENDENT_H
#define SM64_THE_MUTANT_LIMINALITY_DEPENDENT_H
#include "ConstraintState.h"
#include "Constraint.h"
#include "Diagnosable.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

template <typename T>
struct Scalar {
private:
    unsigned int lastWritten; // No use las diferencias para calcular prioridad, use relaciones (>, <)
    int priority; // maybe move to scalar
    std::vector<Constraint*> observers = std::vector<Constraint*>();
    friend class Variable;
protected:
    T value;
public:

    /*
    operator T() const { return value; }
    operator short int() const {
        if (value > INT16_MAX || value < INT16_MIN) {
            std::cerr << "Scalar component must be in the range of int16_t. VALUE HAS BEEN TRUNCATED." << std::endl;
        }
        return value;
    }
    operator signed char() const {
        if (value > INT8_MAX || value < INT8_MIN) {
            std::cerr << "Scalar component must be in the range of int8_t. VALUE HAS BEEN TRUNCATED." << std::endl;
        }
        return value;
    }
*/

    const T& get(const Constraint* key = nullptr) const {
        bool contradiction = false;
        for (auto constraint : observers) {
            if (constraint->state == ConstraintState::VIOLATED) {
                contradiction = true;
                break;
            }
        }
        if (contradiction) {
            for (auto* c : observers) {
                if (c == key) { return value; }
            }
            assert("Dependent has a contradiction in its constraints.");
        }
        return value;
    }
    int getPriority() const {
        return priority;
    }

    void set(T n) {
        for (auto* c : observers) {
            c->state = ConstraintState::VIOLATED;
            Diagnosable::addViolatedConstraint(c);
        }
        value = n;
    }

    Scalar& operator=(T n) {
        assert(false && "Direct assignment to Scalar is not allowed. Use set() method instead.");
    }

    friend std::ostream& operator<<(std::ostream& os, const Scalar& a) {
        return os << a.value;
    }

    void addObserver(Constraint* constraint) {
        observers.push_back(constraint);
    }
};

//Discrete

struct ItemCount : Scalar<int> {
    ItemCount(int n) { value = n; }
};

struct EnumIndex : Scalar<int> {
    EnumIndex(int n) { value = n; }
};

//Continuous

struct Theta : Scalar<float> {
    Theta(float n) { value = n; }
};

struct Length : Scalar<float> {
    Length(float n) { value = n; }
};

struct VAxis : Scalar<float> {
    VAxis(float n) { value = n; }
};

class Variable {
public:
    const std::vector<void*> scalars;

    Variable(const std::vector<void *> & scalars, int priority) : scalars(scalars) {
        for (auto scalar : scalars) {
            if (auto* s = static_cast<Scalar<float>*>(scalar)) {
                s->priority = priority;
            } else if (auto* s = static_cast<Scalar<int>*>(scalar)) {
                s->priority = priority;
            }
        }
    }/*
    void setPriority(int newPriority) {
        for (auto scalar : scalars) {
            if (auto* s = static_cast<Scalar<float>*>(scalar)) {
                s->priority = newPriority;
            } else if (auto* s = static_cast<Scalar<int>*>(scalar)) {
                s->priority = newPriority;
            }
        }
    }*/

    float getModificationCost() {
        float cost = 0.0f;
        for (auto scalar : scalars) {
            //cost += reinterpret_cast<Scalar<float>*>(scalar)->observers.size() * G_GeneralObserverCountCost;
        }
        return cost;
    }
};

struct VVector3 : Variable {
    VVector3(VAxis& x, VAxis& y, VAxis& z, int priority) : Variable({&x, &y, &z}, priority) {}
};

struct SphericalCoordinates : Variable {
    SphericalCoordinates(Theta& theta, Theta& phi, Length& radius, int priority) : Variable({&theta, &phi, &radius}, priority) {}
};

struct SphericalDirection : Variable {
    SphericalDirection(Theta& theta, Theta& phi, int priority) : Variable({&theta, &phi}, priority) {}
};

struct DirectionAndSeparation : Variable {
    DirectionAndSeparation(Theta& theta, Theta& phi, Theta& angularSeparation, int priority) : Variable({&theta, &phi, &angularSeparation}, priority) {}
};

#endif // SM64_THE_MUTANT_LIMINALITY_DEPENDENT_H
