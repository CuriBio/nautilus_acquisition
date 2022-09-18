#include<iostream>

#include <spdlog/spdlog.h>
#include <CameraInterface.h>
#include <AcquisitionInterface.h>
#include <FrameInterface.h>

#include <pm/Camera.h>
#include <pm/Frame.h>
#include <pm/Acquisition.h>


template<AcquisitionConcept A>
void run(std::shared_ptr<A> acq) {
    spdlog::info("Starting acquisition");
    acq->Start();
    acq->WaitForStop();
}

using pmCamera = Camera<pm::Camera, pm::Frame>;
using pmAcquisition = Acquisition<pm::Acquisition, pm::Camera, pm::Frame>;

int main(int argc, char* argv[]) {
    spdlog::info("Running");
    ExpSettings cs {
        .expTimeMS = 2,
        .acqMode = AcqMode::LiveCircBuffer,
        .trigMode = EXT_TRIG_INTERNAL,
        .expMode = EXT_TRIG_INTERNAL,
        .frameCount = 250,
    };

    std::shared_ptr<pmCamera> camera = std::make_shared<pmCamera>();
    camera->Open(0);
    camera->SetupExp(cs);

    std::shared_ptr<pmAcquisition> acquisition = std::make_shared<pmAcquisition>(camera);
    run(acquisition);
}
