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
 * @file  main.cpp
 *
 * @brief Main entrypoint into the nautilus application.
 *********************************************************************/
#include <filesystem>
#include <fmt/chrono.h>
#include <iostream>
#include <stdlib.h>

#include <cxxopts.hpp>
#include <QtWidgets/QApplication>

#include <toml.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/logger.h>
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#ifdef _WIN
#include <Windows.h>
#endif

#include "banner.h"
#include "config.h"
#include "mainwindow.h"
#include <NIDAQmx_wrapper.h>
#include <interfaces/CameraInterface.h>


/*
 * Entry point for nautilus, creates camera/acquisition and sets initial settings.
 *
 * @param argc The number of cli arguments.
 * @param argv Array of pointers to cli arguments.
 */
int main(int argc, char* argv[]) {
    std::cout << banner << std::endl;

    //get user profile path
    std::filesystem::path userProfile{"/Users"};
    char* up = getenv("USERPROFILE");
    if (up != nullptr) {
        userProfile = std::string(up);
    }

    //create AppData directory for log files/config
    std::filesystem::path configPath{fmt::format("{}/AppData/Local/Nautilus", userProfile.string())};
    std::filesystem::path configFile{fmt::format("{}/AppData/Local/Nautilus/nautilus.toml", userProfile.string())};

    std::time_t ts = std::time(nullptr);
    std::string logfile = fmt::format("{}/{:%F_%H%M%S}_nautilus.log", configPath.string(), fmt::localtime(ts));

    auto stderr_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logfile, true);

    std::vector<spdlog::sink_ptr> sinks{stderr_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>("nautilus", std::begin(sinks), std::end(sinks));
    spdlog::set_default_logger(logger);

    spdlog::info("Nautilus Version: {}", version);

    if (!std::filesystem::exists(configPath.string())) {
        spdlog::info("Creating {}", configPath.string());
        std::filesystem::create_directory(configPath.string());
    }

    if (!std::filesystem::exists(configFile)) {
        spdlog::info("Creating {}", configFile.string());
        auto cfg = toml::parse("nautilus.toml");
        std::ofstream outf;
        outf.open(configFile.string());
        outf << cfg << std::endl;
    }
    spdlog::info("Reading config file {}", configFile.string());
    auto config = toml::parse(configFile.string());


    cxxopts::Options options("Nautilus", "CuriBio");
    options.add_options()
      ("a,no_autocb", "Disable auto contrast/brightness for live view", cxxopts::value<bool>())
      ("b,buffers", "Number of buffers", cxxopts::value<uint32_t>())
      ("c,stage_com_port", "COM port for stage controller", cxxopts::value<std::string>())
      ("d,duration", "Acquisition duration", cxxopts::value<double>())
      ("e,exposure_mode", "Camera exposure mode", cxxopts::value<int>())
      ("f,fps", "Frames Per Second", cxxopts::value<double>())
      ("l,led", "LED intensity", cxxopts::value<double>())
      ("m,trigger_mode", "Camera trigger mode", cxxopts::value<int>())
      ("o,outdir", "Output directory", cxxopts::value<std::string>())
      ("p,prefix", "Output file prefix", cxxopts::value<std::string>())
      ("s,storage_type", "Storage type", cxxopts::value<int>())
      ("t,spdtable", "Speed table index", cxxopts::value<uint16_t>())
      ("v,max_voltage", "LED controller max voltage", cxxopts::value<double>())
      ("ni_dev", "Name of NIDAQmx device to use for LED control", cxxopts::value<std::string>())
      ("n,no_gui", "Disable GUI")
      ("test_img", "Use test image", cxxopts::value<std::string>())
      ("version", "Nautilus version")
      ("h,help", "Usage")
      ;
    auto userargs = options.parse(argc, argv);

    //Return version only, this needs to happen before anything
    //else is printed because github action will use it to
    //get the current built version for tagging
    if (userargs.count("version")) {
        std::cout << version << std::endl;
        exit(0);
    }


    if (userargs.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    std::string path = toml::find_or<std::string>(config, "nautilus", "outdir", std::string("E:\\"));
    if (userargs.count("outdir")) {
        path = userargs["outdir"].as<std::string>();
    }
    spdlog::info("Output directory: {}", path);


    auto prefix = toml::find_or<std::string>(config, "nautilus", "prefix", std::string("default_"));
    if (userargs.count("prefix")) {
        prefix = userargs["prefix"].as<std::string>();
    }
    spdlog::info("File prefix: {}", prefix);


    double fps = toml::find_or<double>(config, "acquisition", "fps", 10.0);
    if (userargs.count("fps")) {
        fps = userargs["fps"].as<double>();
    }
    spdlog::info("FPS: {}", fps);


    double duration = toml::find_or<double>(config, "acquisition", "duration", 1.0);
    if (userargs.count("duration")) {
        duration = userargs["duration"].as<double>();
    }
    spdlog::info("Duration (s): {}", duration);


    uint32_t bufferCount = toml::find_or<uint32_t>(config, "acquisition", "buffers", 0);
    if (userargs.count("buffers")) {
        bufferCount = userargs["buffers"].as<uint32_t>();
    }
    spdlog::info("Buffer count: {}", bufferCount);


    uint32_t frameCount = static_cast<uint32_t>(duration * fps);
    spdlog::info("Frame count: {}", frameCount);


    uint16_t spdtable = toml::find_or<uint16_t>(config, "device", "photometrics", "speed_table_index", 0);
    if (userargs.count("spdtable")) {
        spdtable = userargs["spdtable"].as<uint16_t>();
    }
    spdlog::info("Speed table index: {}", spdtable);


    double ledIntensity = toml::find_or<double>(config, "acquisition", "led_intensity", 50.0);
    if (userargs.count("led")) {
        ledIntensity = userargs["led"].as<double>();
    }
    spdlog::info("LED intensity: {}", ledIntensity);


    double expTimeMS = 1000 * (1.0 / fps);
    spdlog::info("Exposure time (ms): {}", expTimeMS);


    double maxVoltage = toml::find_or<double>(config, "device", "nidaqmx", "max_voltage", 1.4);
    if (userargs.count("max_voltage")) {
        maxVoltage = userargs["max_voltage"].as<double>();
    }
    spdlog::info("Max voltage: {}", maxVoltage);


    std::string nidaqmx = toml::find_or<std::string>(config, "device", "nidaqmx", "device", std::string("Dev2"));
    if (userargs.count("ni_dev")) {
        nidaqmx = userargs["ni_dev"].as<std::string>();
    }
    spdlog::info("NI Dev: {}", nidaqmx);


    bool autoConBright = toml::find_or<bool>(config, "nautilus", "auto_contrast_brightness", true);
    if (userargs.count("no_autocb")) {
        autoConBright = false;
    }
    spdlog::info("Disable auto contrast/brightness: {}", !autoConBright);


    std::string stageComPort = toml::find_or<std::string>(config, "device", "tango", "com", std::string("COM3"));
    if (userargs.count("stage_com_port")) {
        stageComPort = userargs["stage_com_port"].as<std::string>();
    }
    spdlog::info("Stage com port: {}", stageComPort);


    bool vflip = toml::find_or<bool>(config, "acquisition", "live_view", "vflip", false);
    spdlog::info("Live view vertical flip: {}", vflip);


    bool hflip = toml::find_or<bool>(config, "acquisition", "live_view", "hflip", false);
    spdlog::info("Live view horizontal flip: {}", hflip);


    uint16_t s1 = toml::find_or<uint16_t>(config, "acquisition", "region", "s1", 800);
    uint16_t s2 = toml::find_or<uint16_t>(config, "acquisition", "region", "s2", 2399);
    uint16_t sbin = toml::find_or<uint16_t>(config, "acquisition", "region", "sbin", 1);
    spdlog::info("Acquisition region s1: {}, s2: {}, sbin: {}", s1, s2, sbin);

    uint16_t p1 = toml::find_or<uint16_t>(config, "acquisition", "region", "p1", 1000);
    uint16_t p2 = toml::find_or<uint16_t>(config, "acquisition", "region", "p2", 2199);
    uint16_t pbin = toml::find_or<uint16_t>(config, "acquisition", "region", "pbin", 1);
    spdlog::info("Acquisition region p1: {}, p2: {}, pbin: {}", p1, p2, pbin);

    Region rgn = {
        .s1 = s1, .s2 = s2, .sbin = sbin,
        .p1 = p1, .p2 = p2, .pbin = pbin
    };


    std::string testImgPath = "";
    if (userargs.count("test_img")) {
        testImgPath = userargs["test_img"].as<std::string>();
    }
    spdlog::info("Test image disabled");


    int storage_type = toml::find_or<int>(config, "acquisition", "storage_type", 0);
    if (userargs.count("storage_type")) {
        storage_type = userargs["storage_type"].as<int>();
    }

    StorageType storageType;
    switch (storage_type) {
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


    int16_t triggerMode = toml::find_or<int16_t>(config, "device", "photometrics", "trigger_mode", 0);
    if (userargs.count("trigger_mode")) {
        triggerMode = userargs["trigger_mode"].as<int16_t>();
    }

    switch (triggerMode) {
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


    int16_t exposureMode = toml::find_or<int16_t>(config, "device", "photometrics", "exposure_mode", 5);
    if (userargs.count("exposure_mode")) {
        exposureMode = userargs["exposure_mode"].as<int16_t>();
    }

    switch (exposureMode) {
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

    std::vector<double> lineTimes = {
        toml::find_or<double>(config, "device", "kinetix", "line_read_times", "sensitivity", 3.53125),
        toml::find_or<double>(config, "device", "kinetix", "line_read_times", "speed", 0.625),
        toml::find_or<double>(config, "device", "kinetix", "line_read_times", "dynamic_range", 3.75),
        toml::find_or<double>(config, "device", "kinetix", "line_read_times", "sub_electron", 60.1)
    };
    spdlog::info("Line times, sensitivity: {}, speed: {}, dynamic_range: {}, sub-electron: {}", lineTimes[0], lineTimes[1], lineTimes[2], lineTimes[3]);

    bool autoTile = toml::find_or<bool>(config, "acquisition", "auto_tile", false);
    uint8_t rows = toml::find_or<uint8_t>(config, "acquisition", "rows", 2);
    uint8_t cols = toml::find_or<uint8_t>(config, "acquisition", "cols", 3);
    spdlog::info("Auto tile: {}, rows: {}, cols: {}", autoTile, rows, cols);

    if (!userargs.count("no_gui")) {
        spdlog::info("Gui mode: {}", true);
        QApplication app(argc, argv);

        Config params {
            .version = version,
            .path = path,
            .prefix = prefix,
            .niDev = nidaqmx,
            .testImgPath = testImgPath,
            .fps = fps,
            .duration = duration,
            .expTimeMs = expTimeMS,
            .spdtable = spdtable,
            .ledIntensity = ledIntensity,
            .bufferCount = bufferCount,
            .frameCount = frameCount,
            .storageType = storageType,
            .triggerMode = triggerMode,
            .exposureMode = exposureMode,
            .maxVoltage = maxVoltage,
            .noAutoConBright = autoConBright,
            .autoTile = autoTile,
            .vflip = vflip,
            .hflip = hflip,
            .rows = rows,
            .cols = cols,
            .rgn = rgn,
            .stageComPort = stageComPort,
            .configFile = configFile.string(),
            .config = config,
            .lineTimes = lineTimes
        };

        MainWindow win(params);

        win.resize(800, 640);
        win.setVisible(true);
        win.Initialize();

        return app.exec();
    } else {
        spdlog::info("Gui Mode: {}", false);
        std::shared_ptr<pmCamera> camera = std::make_shared<pmCamera>();

        CameraInfo m_camInfo;

        camera->Open(0);
        CameraInfo info = camera->GetInfo();
        for(auto& i : info.spdTable) {
            spdlog::info("port: {}, pixTimeNs: {}, spdIndex: {}, gainIndex: {}, gainName: {}, bitDepth: {}", i.portName, i.pixTimeNs, i.spdIndex, i.gainIndex, i.gainName, i.bitDepth);
        }

        ExpSettings m_expSettings {
            .acqMode = AcqMode::LiveCircBuffer,
            .workingDir = path,
            .acquisitionDir = path,
            .filePrefix = prefix,
            .region = {
                .s1 = uns16(s1), .s2 = uns16(s2), .sbin = sbin,
                .p1 = uns16(p1), .p2 = uns16(p2), .pbin = pbin
            },
            .storageType = storageType,
            .spdTableIdx = spdtable,
            .expTimeMS = static_cast<uint32_t>(expTimeMS),
            .trigMode = triggerMode,
            .expModeOut = exposureMode,
            .frameCount = frameCount,
            .bufferCount = bufferCount
        };

        camera->SetupExp(m_expSettings);

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

