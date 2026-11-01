//
// Created by Luis Alvarez on 30/10/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_STAGEDBATCH_H
#define SM64_THE_MUTANT_LIMINALITY_STAGEDBATCH_H
#pragma once
#include "NonRecoverableContradictionException.h"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <string>
#include <tuple>
#include <vector>

// Etapas de construcción de un lote de objetos. Solo se avanza en este orden, una vez cada una.
enum class BuildStage { Instantiated, Linked, Constrained };

// Un participante debe poder decir si todos sus enlaces mutuos ya están completos.
template <typename T>
concept Stageable = requires(const T& t) {
    { t.isLinked() } -> std::convertible_to<bool>;
};

// Impone el orden Instancia -> Enlace -> Restricciones sobre un conjunto de objetos ya creados.
// Ninguna etapa comienza para ningún participante hasta que la anterior terminó para todos.
// Es de un solo uso: una instancia por operación. No crea objetos ni sabe cómo se enlazan.
// Si un participante define constrain(), se llama en la etapa de restricciones; si no, se omite.
// isLinked() y constrain() pueden ser privados si el tipo declara
// `template <Stageable... Ts> friend class StagedBatch;`.
template <Stageable... Ts>
class StagedBatch {
    std::tuple<std::vector<Ts*>...> participants;
    BuildStage stage = BuildStage::Instantiated;

    void requireStage(BuildStage expected, const std::string& operation) const {
        assert(stage == expected && "StagedBatch: operation called out of order.");
        if (stage != expected) {
            throw NonRecoverableContradictionException("StagedBatch::" + operation + " called out of order.");
        }
    }

    bool allLinked() const {
        return std::apply([](const auto&... groups) {
            return (std::ranges::all_of(groups, [](const auto* p) { return p->isLinked(); }) && ...);
        }, participants);
    }

public:
    // Solo antes de enlazar: el lote debe conocer todos los participantes antes de validar.
    template <typename T>
    void add(T& obj) {
        requireStage(BuildStage::Instantiated, "add");
        std::get<std::vector<T*>>(participants).push_back(&obj);
    }

    // Instancia -> Enlace. linkFn realiza los enlaces; después todos los participantes deben estar enlazados.
    template <typename F>
    void link(F&& linkFn) {
        requireStage(BuildStage::Instantiated, "link");
        std::forward<F>(linkFn)();
        const bool linked = allLinked();
        assert(linked && "StagedBatch: a participant is not fully linked after the link stage.");
        if (!linked) {
            throw NonRecoverableContradictionException("StagedBatch: a participant is not fully linked after the link stage.");
        }
        stage = BuildStage::Linked;
    }

    // Enlace -> Restricciones.
    void constrain() {
        requireStage(BuildStage::Linked, "constrain");
        std::apply([](auto&... groups) {
            (([](auto& group) {
                for (auto* p : group) {
                    if constexpr (requires { p->constrain(); }) {
                        p->constrain();
                    }
                }
            }(groups)), ...);
        }, participants);
        stage = BuildStage::Constrained;
    }

    BuildStage getStage() const { return stage; }
};

#endif // SM64_THE_MUTANT_LIMINALITY_STAGEDBATCH_H
