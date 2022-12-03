#ifndef PARTASK_INTERFACE_H
#define PARTASK_INTERFACE_H
#include <concepts>

template<typename T>
concept TaskConcept = requires(T c) {
    { c.Run(uint8_t(), uint8_t()) } -> std::same_as<void>;
};
#endif //PARTASK_INTERFACE_H
