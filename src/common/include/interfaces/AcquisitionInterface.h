#ifndef ACQUISITION_INTERFACE_H
#define ACQUISITION_INTERFACE_H
#include <concepts>
#include <memory>

#include "CameraInterface.h"
#include "FrameInterface.h"
#include "ColorConfigInterface.h"

enum AcquisitionState {
    AcqStopped,
    AcqLiveScan,
    AcqCapture
};

template<typename T, typename F, template<typename C> typename Color, typename Cfg>
concept AcquisitionConcept = FrameConcept<F> and ColorConfigConcept<Color<Cfg>> and requires(T c, std::shared_ptr<F> pframe, const Color<Cfg>* cctx) {
    { c.Start(bool(), double(), cctx) } -> std::same_as<bool>;
    { c.Abort() } -> std::same_as<bool>;
    { c.WaitForStop() } -> std::same_as<void>;
    { c.IsRunning() } -> std::same_as<bool>;
    { c.ProcessNewFrame(pframe) } -> std::same_as<bool>;
    { c.GetLatestFrame() } -> std::same_as<std::shared_ptr<F>>;
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
