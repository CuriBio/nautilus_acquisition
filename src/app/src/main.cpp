#include <filesystem>
#include<iostream>
#include <stdlib.h>

#include <cxxopts.hpp>
#include <QtWidgets/QApplication>
#include <spdlog/spdlog.h>

#ifdef _WIN
#include <Windows.h>
#endif

#include "banner.h"
#include "mainwindow.h"
#include <NIDAQmx_wrapper.h>

#define VERSION "0.0.2"


int main(int argc, char* argv[]) {
    std::filesystem::path userProfile{"/Users"};
    char* up = getenv("USERPROFILE");
    if (up != nullptr) {
        userProfile = std::string(up);
    }

    cxxopts::Options options("Nautilus", "CuriBio");
    options.add_options()
      ("a,no_autocb", "Disable auto contrast/brightness for live view", cxxopts::value<bool>()->default_value("false"))
      ("b,buffers", "Number of buffers", cxxopts::value<uint32_t>()->default_value("0"))
      ("d,duration", "Acquisition duration", cxxopts::value<double>()->default_value("1.0"))
      ("e,exposure_mode", "Camera exposure mode", cxxopts::value<int>()->default_value("5"))
      ("f,fps", "Frames Per Second", cxxopts::value<double>()->default_value("10.0"))
      ("l,led", "LED intensity", cxxopts::value<double>()->default_value("50.0"))
      ("m,trigger_mode", "Camera trigger mode", cxxopts::value<int>()->default_value("0"))
      ("n,no_gui", "Disable GUI", cxxopts::value<bool>()->default_value("false")) // a bool parameter
      ("o,outdir", "Output directory", cxxopts::value<std::string>()->default_value(userProfile.string()))
      ("p,prefix", "Output file prefix", cxxopts::value<std::string>()->default_value("default_"))
      ("s,storage_type", "Storage type", cxxopts::value<int>()->default_value("0"))
      ("t,spdtable", "Speed table index", cxxopts::value<uint16_t>()->default_value("1"))
      ("v,max_voltage", "LED controller max voltage", cxxopts::value<double>()->default_value("1.4"))
      ("ni_dev", "Name of NIDAQmx device to use for LED control", cxxopts::value<std::string>()->default_value("Dev2"))
      ("debug", "Enable debug console", cxxopts::value<bool>()->default_value("false"))
      ("test_img", "Use test image", cxxopts::value<std::string>())
      ("h,help", "Usage")
      ;
    auto userargs = options.parse(argc, argv);

#ifdef _WIN32
    if (!userargs["debug"].as<bool>()) {
        if (AttachConsole(ATTACH_PARENT_PROCESS)) {
            freopen("CONOUT$", "w", stdout);
            freopen("CONOUT$", "w", stderr);
        }
    }
#endif
    std::cout << banner << std::endl;
    spdlog::info("Nautilus Version: {}", VERSION);


    if (userargs.count("help")) {
      std::cout << options.help() << std::endl;
      exit(0);
    }

    std::string path = userargs["outdir"].as<std::string>();
    spdlog::info("Output directory: {}", path);

    std::string prefix = userargs["prefix"].as<std::string>();
    spdlog::info("File prefix: {}", prefix);

    double fps = userargs["fps"].as<double>();
    spdlog::info("FPS: {}", fps);

    double duration = userargs["duration"].as<double>();
    spdlog::info("Duration (s): {}", duration);

    uint32_t bufferCount = userargs["buffers"].as<uint32_t>();
    spdlog::info("Buffer count: {}", bufferCount);

    uint32_t frameCount = static_cast<uint32_t>(duration * fps);
    spdlog::info("Frame count: {}", frameCount);

    uint16_t spdtable = userargs["spdtable"].as<uint16_t>();
    spdlog::info("Speed table index: {}", spdtable);

    double ledIntensity = userargs["led"].as<double>();
    spdlog::info("LED intensity: {}", ledIntensity);

    double expTimeMS = 1000 * (1.0 / fps);
    spdlog::info("Exposure time (ms): {}", expTimeMS);

    double maxVoltage = userargs["max_voltage"].as<double>();
    spdlog::info("Max voltage: {}", maxVoltage);

    bool noAutoConBright = userargs["no_autocb"].as<bool>();
    spdlog::info("Disable auto contrast/brightness: {}", noAutoConBright);

    std::string testImgPath = "";
    if (userargs.count("test_img")) {
        testImgPath = userargs["test_img"].as<std::string>();
    }

    std::string niDev = userargs["ni_dev"].as<std::string>();
    spdlog::info("NI Dev: {}", niDev);

    StorageType storageType;
    switch (userargs["storage_type"].as<int>()) {
        case 0:
            spdlog::info("Storage type: {}", "tiff");
            storageType = StorageType::Tiff;
            break;
        case 1:
            spdlog::info("Storage type: {}", "tiff stack");
            storageType = StorageType::TiffStack;
            break;
        case 2:
            spdlog::info("Storage type: {}", "big tiff");
            storageType = StorageType::BigTiff;
            break;
        default:
            spdlog::error("Invalid storage type");
            exit(0);
    }

    int16_t triggerMode;
    switch (userargs["trigger_mode"].as<int>()) {
        case 0:
            spdlog::info("Trigger mode: {}", "EXT_TRIG_INTERNAL");
            triggerMode = EXT_TRIG_INTERNAL;
            break;
        case 1:
            spdlog::info("Trigger mode: {}", "EXT_TRIG_TRIG_FIRST");
            triggerMode = EXT_TRIG_TRIG_FIRST;
            break;
        case 2:
            spdlog::info("Trigger mode: {}", "EXT_TRIG_EDGE_RISING");
            triggerMode = EXT_TRIG_EDGE_RISING;
            break;
        case 3:
            spdlog::info("Trigger mode: {}", "EXT_TRIG_LEVEL");
            triggerMode = EXT_TRIG_LEVEL;
            break;
        case 4:
            spdlog::info("Trigger mode: {}", "EXT_TRIG_SOFTWARE_FIRST");
            triggerMode = EXT_TRIG_SOFTWARE_FIRST;
            break;
        case 5:
            spdlog::info("Trigger mode: {}", "EXT_TRIG_SOFTWARE_EDGE");
            triggerMode = EXT_TRIG_SOFTWARE_EDGE;
            break;
        case 6:
            spdlog::info("Trigger mode: {}", "EXT_TRIG_LEVEL_OVERLAP");
            triggerMode = EXT_TRIG_LEVEL_OVERLAP;
            break;
        case 7:
            spdlog::info("Trigger mode: {}", "EXT_TRIG_LEVEL_PULSED");
            triggerMode = EXT_TRIG_LEVEL_PULSED;
            break;
        default:
            spdlog::error("Invalid trigger mode");
            exit(0);
    }

    int16_t exposureMode;
    switch (userargs["exposure_mode"].as<int>()) {
        case 0:
            spdlog::info("Exposure out mode: {}", "EXPOSE_OUT_FIRST_ROW");
            exposureMode = EXPOSE_OUT_FIRST_ROW;
            break;
        case 1:
            spdlog::info("Exposure out mode: {}", "EXPOSE_OUT_ALL_ROWS");
            exposureMode = EXPOSE_OUT_ALL_ROWS;
            break;
        case 2:
            spdlog::info("Exposure out mode: {}", "EXPOSE_OUT_ANY_ROW");
            exposureMode = EXPOSE_OUT_ANY_ROW;
            break;
        case 3:
            spdlog::info("Exposure out mode: {}", "EXPOSE_OUT_ROLLING_SHUTTER");
            exposureMode = EXPOSE_OUT_ROLLING_SHUTTER;
            break;
        case 4:
            spdlog::info("Exposure out mode: {}", "EXPOSE_OUT_LINE_TRIGGER");
            exposureMode = EXPOSE_OUT_LINE_TRIGGER;
            break;
        case 5:
            spdlog::info("Exposure out mode: {}", "EXPOSE_OUT_GLOBAL_SHUTTER");
            exposureMode = EXPOSE_OUT_GLOBAL_SHUTTER;
            break;
        case 6:
            spdlog::info("Exposure out mode: {}", "MAX_EXPOSE_OUT_MODE");
            exposureMode = MAX_EXPOSE_OUT_MODE;
            break;
        default:
            spdlog::error("Invalid exposure mode");
            break;
    }

    if (!userargs["no_gui"].as<bool>()) {
        spdlog::info("Gui mode: {}", true);
        QApplication app(argc, argv);

        MainWindow win(
            path,
            prefix,
            niDev,
            testImgPath,
            fps,
            duration,
            expTimeMS,
            spdtable,
            ledIntensity,
            bufferCount,
            frameCount,
            storageType,
            triggerMode,
            exposureMode,
            maxVoltage,
            noAutoConBright
        );

        win.resize(800, 640);
        win.setVisible(true);
        win.Initialize();

        return app.exec();
    } else {
        spdlog::info("Gui Mode: {}", false);
        std::shared_ptr<pmCamera> camera = std::make_shared<pmCamera>();

        ExpSettings m_expSettings;
        CameraInfo m_camInfo;

        camera->Open(0);
        CameraInfo info = camera->GetInfo();
        for(auto& i : info.spdTable) {
            spdlog::info("port: {}, pixTimeNs: {}, spdIndex: {}, gainIndex: {}, gainName: {}, bitDepth: {}", i.portName, i.pixTimeNs, i.spdIndex, i.gainIndex, i.gainName, i.bitDepth);
        }

        camera->SetupExp(ExpSettings {
            .acqMode = AcqMode::LiveCircBuffer,
            .filePath = path,
            .filePrefix = prefix,
            .region = {
                .s1 = 0, .s2 = uns16(info.sensorResX - 1), .sbin = 1,
                .p1 = 0, .p2 = uns16(info.sensorResY - 1), .pbin = 1
            },
            .storageType = storageType,
            .spdTableIdx = spdtable,
            .expTimeMS = static_cast<uint32_t>(expTimeMS),
            .trigMode = triggerMode,
            .expModeOut = exposureMode,
            .frameCount = frameCount,
            .bufferCount = bufferCount
        });

        std::shared_ptr<pmAcquisition> acquisition = std::make_shared<pmAcquisition>(camera);
        acquisition->Start(true, 0.0, nullptr);
        acquisition->WaitForStop();
    }

    return 0;
}

#ifdef _WIN
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd) {
    return main(__argc, __argv);
}
#endif

