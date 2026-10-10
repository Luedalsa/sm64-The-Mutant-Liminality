//
// Created by Luis Alvarez on 10/10/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_CONSTRAINTVIOLATIONSEVERITY_H
#define SM64_THE_MUTANT_LIMINALITY_CONSTRAINTVIOLATIONSEVERITY_H

enum class ConstraintViolationSeverity {
    TotallyUnrepresentable,            // La violación ha provocado que los display list y las colisiones entren en estados peligrosos donde no se puede garantizar la integridad de la geometría, aparecen ángulos extraños o Mario cae al vacío.
    GeometricallyUnrepresentable,      // La violación ha provocado que la geometría no pueda representarse correctamente, pero los display list y las colisiones aún son válidos. Una restricción con esta violación toma prestado las superficies donde nació para representarse temporalmente.
    SemanticallyUnrepresentable,       // La violación ha provocado que la semántica de la geometría no pueda representarse correctamente, pero los display list y las colisiones aún son válidos. Una restricción con esta violación fusiona texturas y materiales para representar la geometría de manera aproximada.
    FullyRenderable                    // La violación no ha provocado ningún problema en la representación de la geometría, los display list y las colisiones son válidos. La violación de estas restricciones informan problemas de semántica, tamaño, tensión, etc. que no afectan la representación de la geometría.
};

#endif // SM64_THE_MUTANT_LIMINALITY_CONSTRAINTVIOLATIONSEVERITY_H
