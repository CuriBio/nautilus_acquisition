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
 * @file  AcquisitionInterface.h
 * 
 * @brief Definition of the AcquisitionInterface concept.
 *********************************************************************/
#ifndef ACQUISITION_INTERFACE_H
#define ACQUISITION_INTERFACE_H
#include <concepts>
#include <functional>
#include <memory>

#include "CameraInterface.h"
#include "FrameInterface.h"
#include "ColorConfigInterface.h"

/*
* @brief Acquisition state
*/
enum AcquisitionState {
    AcqStopped,
    AcqLiveScan,
    AcqCapture
};


/*
* Defines acquisition concept, any class that needs to fulfill this interface must
* implement all of the methods.
*
* @tparam T Type param.
* @tparam F FrameConcept type.
* @tparam Color Template type for color config concept.
* @tparam Cfg Color config type.
*/
template<typename T, typename F, template<typename C> typename Color, typename Cfg>
concept AcquisitionConcept = FrameConcept<F> and ColorConfigConcept<Color<Cfg>> and requires(T c, F* pframe, const Color<Cfg>* cctx, std::function<void(size_t)> progressCB) {
    { c.Start(bool(), progressCB, double(), cctx) } -> std::same_as<bool>;
    { c.Stop() } -> std::same_as<bool>;
    { c.WaitForStop() } -> std::same_as<void>;
    { c.IsRunning() } -> std::same_as<bool>;
    { c.GetLatestFrame() } -> std::same_as<F*>;
    { c.GetState() } -> std::same_as<AcquisitionState>;
};

template<
    template<typename F, typename C> typename Acq,
    template<ColorConfigConcept> class Color, typename Cfg,
    template<FrameConcept> typename Camera,
    FrameConcept Frame
> requires(CameraConcept<Camera<Frame>, Frame>, AcquisitionConcept<Acq<Frame, Color<Cfg>>, Frame, Color, Cfg>)
struct Acquisition : public Acq<Frame, Color<Cfg>> {
  Acquisition(std::shared_ptr<Camera<Frame>> camera) : Acq<Frame, Color<Cfg>>(camera) {}
};


#endif //ACQUISITION_INTERFACE_H
