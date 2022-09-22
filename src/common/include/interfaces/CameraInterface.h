#ifndef CAMERA_INTERFACE_H
#define CAMERA_INTERFACE_H
#include <string>
#include <memory>
#include <vector>

#include "FrameInterface.h"
#include "BitmapFormat.h"

enum class AcqMode : int32_t {
    SnapSequence,
    SnapCircBuffer,
    SnapTimeLapse,
    LiveCircBuffer,
    LiveTimeLapse,
};

struct Region {
    uint16_t s1{0};
    uint16_t s2{0};
    uint16_t sbin{1};
    uint16_t p1{0};
    uint16_t p2{0};
    uint16_t pbin{1};
};

struct SpdTable {
    int32_t gainIndex{1};
    int16_t bitDepth{0};
    std::string gainName{};

    int32_t spdIndex{0};
    uint16_t pixTimeNs{1};

    int32_t spdTabPort{0};
    std::string portName{};
};

struct CameraInfo {
    std::string name{""};
    std::string driver{""};
    std::string firmware{""};
    std::string chip{""};

    uint16_t sensorResX{0}; //sensor width
    uint16_t sensorResY{0}; //sensor height

    std::vector<SpdTable> spdTable{};
};

struct ExpSettings {
    AcqMode acqMode;
    std::string fileName;

    Region region {0};
    ImageFormat imgFormat {ImageFormat::Mono16};

    uint16_t spdTableIdx{0};

    uint32_t expTimeMS{0};
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
    { c.GetInfo() } -> std::same_as<CameraInfo&>;
    { c.SetupExp(pExpSettings) } -> std::same_as<bool>;
    { c.StartExp(vptr, vptr) } -> std::same_as<bool>;
    { c.StopExp() } -> std::same_as<bool>;
    { c.GetLatestFrame(pframe) } -> std::same_as<bool>;
    { c.GetFrameExpTime(uint32_t()) } -> std::same_as<uint32_t>;
};

template<template<FrameConcept F> typename X, typename A> requires(CameraConcept<X<A>, A>)
struct Camera : public X<A> { };

#endif //CAMERA_INTERFACE_H
