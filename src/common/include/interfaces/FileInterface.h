#ifndef FILE_INTERFACE_H
#define FILE_INTERFACE_H
#include <concepts>

#include "FrameInterface.h"

template<typename T, typename F>
concept FileConcept = FrameConcept<F> and requires(T c, T const cc, std::shared_ptr<F> pframe) {
    { c.Open(std::string()) } -> std::same_as<bool>;
    { c.Close() } -> std::same_as<void>;
    { c.WriteFrame(pframe) } -> std::same_as<bool>;
    { cc.IsOpen() } -> std::same_as<bool>;
    { cc.Name() } -> std::same_as<const std::string&>;
};

#endif //FILE_INTERFACE_H
