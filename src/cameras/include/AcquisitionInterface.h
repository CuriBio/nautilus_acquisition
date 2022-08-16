#ifndef ACQUISITION_INTERFACE_H
#define ACQUISITION_INTERFACE_H
#include <concepts>
#include <memory>

#include <CameraInterface.h>
#include <FrameInterface.h>

template<typename T>
concept AcquisitionConcept = requires(T c) {
    { c.Start() } -> std::same_as<bool>;
    { c.Abort() } -> std::same_as<bool>;
    { c.WaitForStop() } -> std::same_as<void>;
    { c.IsRunning() } -> std::same_as<bool>;
};

template<template<AcquisitionConcept> typename C, template<FrameConcept> typename X, typename F> requires(CameraConcept<X<F>, F>)
struct Acquisition : public C<F> {
  Acquisition(std::shared_ptr<X<F>> camera) : C<F>(camera) {}
};


#endif //ACQUISITION_INTERFACE_H
