//
// Created by Luis Alvarez on 21/09/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_NONRECOVERABLECONTRADICTIONEXCEPTION_H
#define SM64_THE_MUTANT_LIMINALITY_NONRECOVERABLECONTRADICTIONEXCEPTION_H
#include <string>
#include <stdexcept>

class NonRecoverableContradictionException : public std::runtime_error {
public:
    NonRecoverableContradictionException(const std::string& message)
        : std::runtime_error("NON RECOVERABLE CONTRADICTION: " + message + "\nThe algorithm reached a non-recoverable state, the operation could not be completed due to failed or unregistered diagnostic checks.") {}
};

#endif // SM64_THE_MUTANT_LIMINALITY_NONRECOVERABLECONTRADICTIONEXCEPTION_H
