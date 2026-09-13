//
// Created by Luis Alvarez on 08/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_RELATION_H
#define SM64_THE_MUTANT_LIMINALITY_RELATION_H

#pragma once
#include <cstdint>
#include <vector>

// "una relación es un puntero a un int32": aquí modelamos ese puntero como un
// identificador (RelationId) que indexa una tabla en el solver. El nivel de
// indirección real (int32_t*) no aporta nada al modelo salvo el ID en sí, así
// que lo mantenemos explícito y le damos nombre para que el intent quede claro.
using RelationId = int32_t;
static constexpr RelationId kInvalidRelation = -1;

enum class ToleranceMode : uint8_t {
    Explicit,       // +/- toleranceValue fijo (ej. relación 7: ±50 unidades)
    DerivedMinMax   // se calcula a partir del min/max de tamaño de sus hijos
                    // en el mapa semántico (ej. relación 1)
};

// Invariante que debe preservarse entre los componentes directos de una
// relación compuesta. SumPreserving == "si uno decrece, otro debe crecer
// para mantener la suma total" (relación 10 en tu ejemplo).
enum class InvariantMode : uint8_t {
    None,
    SumPreserving
};

struct Relation {
    RelationId id = kInvalidRelation;

    // Composición fija: relación 4 = {1, 2, 3}. Vacío si es una relación simple.
    std::vector<RelationId> components;

    // "Relación 1 se compone de x repeticiones de relación 7": components[0]
    // es la unidad repetida, y repeatCount cuenta cuántas instancias vivas
    // (edges) la satisfacen actualmente.
    bool isRepeated = false;
    int  repeatCount = 0;

    ToleranceMode toleranceMode = ToleranceMode::Explicit;
    float toleranceValue = 0.0f;   // usado si Explicit
    float derivedMin = 0.0f;       // llenado si DerivedMinMax
    float derivedMax = 0.0f;       // llenado si DerivedMinMax

    InvariantMode invariant = InvariantMode::None;
    float targetSum = 0.0f;        // usado si invariant == SumPreserving

    float baseSize = 0.0f;         // tamaño nominal de referencia
};

#endif // SM64_THE_MUTANT_LIMINALITY_RELATION_H
