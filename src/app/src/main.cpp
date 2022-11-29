#include<iostream>

#include <spdlog/spdlog.h>

#include<QtWidgets/QApplication>
#include "mainwindow.h"

/* void run(std::shared_ptr<pmAcquisition> acq) { */
/*     spdlog::info("Starting acquisition"); */
/*     //TODO pass color context */
/*     acq->Start(true, 0.0, nullptr); */
/*     acq->WaitForStop(); */
/* } */

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow win;
    win.resize(800, 640);
    win.setVisible(true);
    win.Initialize();

    spdlog::info("Running");
    return app.exec();

    /* std::shared_ptr<pmCamera> camera = std::make_shared<pmCamera>(); */

    /* ExpSettings m_expSettings; */
    /* CameraInfo m_camInfo; */

    /* camera->Open(0); */
    /* CameraInfo info = camera->GetInfo(); */
    /* for(auto& i : info.spdTable) { */
    /*     spdlog::info("port: {}, pixTimeNs: {}, spdIndex: {}, gainIndex: {}, gainName: {}, bitDepth: {}", i.portName, i.pixTimeNs, i.spdIndex, i.gainIndex, i.gainName, i.bitDepth); */
    /* } */

    /* camera->SetupExp(ExpSettings { */
    /*     .acqMode = AcqMode::LiveCircBuffer, */
    /*     .filePath = "C:/Users/Jason/test", */
    /*     .filePrefix = "ss", */
    /*     .region = { */
    /*         .s1 = 0, .s2 = uns16(info.sensorResX - 1), .sbin = 1, */
    /*         .p1 = 0, .p2 = uns16(info.sensorResY - 1), .pbin = 1 */
    /*     }, */
    /*     .imgFormat = ImageFormat::Mono16, */
    /*     .storageType = StorageType::Tiff, */
    /*     .spdTableIdx = 0, */
    /*     .expTimeMS = 2, */
    /*     .trigMode = EXT_TRIG_INTERNAL, */
    /*     .expModeOut = EXPOSE_OUT_GLOBAL_SHUTTER, */
    /*     .frameCount = 1000, */
    /*     .bufferCount = 50 */
    /* }); */

    /* std::shared_ptr<pmAcquisition> acquisition = std::make_shared<pmAcquisition>(camera); */
    /* run(acquisition); */
}
