/*
 * MIT License
 *
 * Copyright (c) 2022 Curi Bio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*********************************************************************
 * @file  CameraInterface.h
 * 
 * Definition of the CameraInterface concept.
 *********************************************************************/
#ifndef CAMERA_INTERFACE_H
#define CAMERA_INTERFACE_H
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "FrameInterface.h"
#include "BitmapFormat.h"

/*
* Acquisition mode enum.
*/
enum class AcqMode : int32_t {
    SnapSequence,
    SnapCircBuffer,
    SnapTimeLapse,
    LiveCircBuffer,
    LiveTimeLapse,
};


/*
* Defines different storage types for an acquisition.
*/
enum StorageType {
    Tiff = 0,
    TiffStack,
    BigTiff,
    Prd
};


/*
* Defines a capture region for an exposure.
*/
struct Region {
    uint16_t s1{0};
    uint16_t s2{0};
    uint16_t sbin{1};
    uint16_t p1{0};
    uint16_t p2{0};
    uint16_t pbin{1};
};


/*
* Structure to hold camera speed table entries.
*/
struct SpdTable {
    int32_t gainIndex{1};
    int16_t bitDepth{0};
    std::string gainName{};

    int16_t spdIndex{0};
    uint16_t pixTimeNs{1};

    int32_t spdTabPort{0};
    std::string portName{};
};


/*
* Structure to hold specific values for a given camera instance.
*/
struct CameraInfo {
    std::string name{""};
    std::string driver{""};
    std::string firmware{""};
    std::string chip{""};

    uint16_t sensorResX{0}; //sensor width
    uint16_t sensorResY{0}; //sensor height
    ImageFormat imageFormat{ImageFormat::Mono16};

    std::vector<SpdTable> spdTable{};
};


/*
* Structure to hold settings specific to a running exposure.
*/
struct ExpSettings {
    AcqMode acqMode;
    std::filesystem::path workingDir;
    std::filesystem::path acquisitionDir;
    std::string filePrefix;

    Region region {0};
    StorageType storageType {StorageType::TiffStack};
    uint16_t spdTableIdx{0};

    uint32_t expTimeMS{0};
    int16_t trigMode{0};
    int16_t expModeOut{0};

    uint32_t frameCount{0};
    uint32_t bufferCount{0};

    float colorWbScaleRed{ 1.0 };
    float colorWbScaleGreen{ 1.0 };
    float colorWbScaleBlue{ 1.0 };
};


/*
* Defines camera concept, any class that needs to fulfill this interface must
* implement all of the methods.
*
* @tparam T Type param.
* @tparam F FrameConcept type.
*/
template<typename T, typename F>
concept CameraConcept = FrameConcept<F> and requires(T c, F* pframe, void* vptr, const ExpSettings& constPExpSettings, ExpSettings& pExpSettings) {
    { c.Open(int8_t()) } -> std::same_as<bool>;
    { c.Close() } -> std::same_as<bool>;
    { c.GetInfo() } -> std::same_as<CameraInfo&>;
    { c.SetupExp(pExpSettings) } -> std::same_as<bool>;
    { c.UpdateExp(constPExpSettings) } -> std::same_as<bool>;
    { c.StartExp(vptr, vptr) } -> std::same_as<bool>;
    { c.StopExp() } -> std::same_as<bool>;
    { c.GetLatestFrame(pframe) } -> std::same_as<bool>;
    { c.GetFrameExpTime(uint32_t()) } -> std::same_as<uint32_t>;
};

template<template<FrameConcept F> typename X, typename A> requires(CameraConcept<X<A>, A>)
struct Camera : public X<A> { };

#endif //CAMERA_INTERFACE_H
