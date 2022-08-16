#ifndef CAMERA_INTERFACE_H
#define CAMERA_INTERFACE_H
//#include <cstdint>
#include<memory>

#include <FrameInterface.h>

enum class AcqMode : int32_t {
    SnapSequence,
    SnapCircBuffer,
    SnapTimeLapse,
    LiveCircBuffer,
    LiveTimeLapse,
};

struct ExpSettings {
    uint32_t expTimeMS{0};
    AcqMode acqMode;
    int16_t trigMode{0};
    int16_t expMode{0};

    uint64_t frameCount{0};
    uint64_t bufferCount{0};

    float colorWbScaleRed{ 1.0 };
    float colorWbScaleGreen{ 1.0 };
    float colorWbScaleBlue{ 1.0 };
};

template<typename T, typename F>
concept CameraConcept = FrameConcept<F> and requires(T c, std::shared_ptr<F> pframe, void* vptr, const ExpSettings& pExpSettings) {
    { c.Open(int8_t()) } -> std::same_as<bool>;
    { c.Close() } -> std::same_as<bool>;
    { c.SetupExp(pExpSettings) } -> std::same_as<bool>;
    { c.StartExp(vptr, vptr) } -> std::same_as<bool>;
    { c.StopExp() } -> std::same_as<bool>;
    { c.GetLatestFrame(pframe) } -> std::same_as<bool>;
    { c.GetFrameExpTime(uint32_t()) } -> std::same_as<uint32_t>;
};

template<template<FrameConcept F> typename X, typename A> requires(CameraConcept<X<A>, A>)
struct Camera : public X<A> { };


#endif //CAMERA_INTERFACE_H
