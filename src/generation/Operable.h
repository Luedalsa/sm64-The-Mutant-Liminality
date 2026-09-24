//
// Created by Luis Alvarez on 19/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_OPERABLE_H
#define SM64_THE_MUTANT_LIMINALITY_OPERABLE_H

enum Operators {
    MITOSIS,
    COPY_FROM,
};

class Operable {
    virtual void operate(Operators op, Operable* other) = 0;
};

#endif // SM64_THE_MUTANT_LIMINALITY_OPERABLE_H
