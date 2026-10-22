//
// Created by Luis Alvarez on 19/09/2026.
//

#include "Diagnosable.h"
std::priority_queue<Constraint*, std::vector<Constraint*>, std::function<bool(Constraint *, Constraint *)>>  Diagnosable::constraints;