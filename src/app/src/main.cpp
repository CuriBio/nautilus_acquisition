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
 * @brief Main entrypoint into the nautilai application.
 *********************************************************************/
#include <deque>
#include <filesystem>
#include <iostream>
#include <stdlib.h>
#include <thread>

#include <cxxopts.hpp>
#include <fmt/chrono.h>
#include <QtWidgets/QApplication>
#include <QFile>
#include <QIcon>

#include <spdlog/spdlog.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <tsl/ordered_map.h>
#include <toml.hpp>

#ifdef _WIN
#include <Windows.h>
#endif

#include "banner.h"
#include "config.h"
#include "mainwindow.h"
#include <NIDAQmx_wrapper.h>
#include <interfaces/CameraInterface.h>


/*
 * Entry point for nautilai, creates camera/acquisition and sets initial settings.
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

    std::filesystem::path logPath = (userProfile / "Documents" / "Nautilai" / "Logs");
    std::time_t ts = std::time(nullptr);
    std::string logfile = fmt::format("{}/{:%F_%H%M%S}_nautilai.log", logPath.string(), fmt::localtime(ts));

    auto stderr_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logfile, true);

    std::vector<spdlog::sink_ptr> sinks{stderr_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>("nautilai", std::begin(sinks), std::end(sinks));
    spdlog::flush_every(std::chrono::seconds(10));
    spdlog::set_default_logger(logger);

    //create AppData directory for config file
    std::filesystem::path configPath = (userProfile / "AppData" / "Local" / "Nautilai");
    std::filesystem::path configFile = (configPath / "nautilai.toml");

    spdlog::info("Nautilai Version: {}", version);

    if (!std::filesystem::exists(configPath.string())) {
        spdlog::info("Creating {}", configPath.string());
        std::filesystem::create_directory(configPath.string());
    }

    if (!std::filesystem::exists(configFile)) {
        spdlog::info("Creating {}", configFile.string());
        auto cfg = toml::parse<toml::preserve_comments, tsl::ordered_map>(std::filesystem::path("nautilai.toml").string());
        std::ofstream outf(configFile.string());
        outf << std::setw(0) << cfg << std::endl;
        outf.close();
    }

    cxxopts::Options options("Nautilai", "CuriBio");
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
      ("version", "Nautilai version")
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

    spdlog::info("Loading config {}", configFile.string());
    std::shared_ptr<Config> config = std::make_shared<Config>(configFile, userProfile, userargs);
    config->version = version;
    config->configFile = configFile.string();

    if (config->configError.empty()) {
        config->Dump();
    }

    if (!std::filesystem::exists(config->backgroundRecordingDir)) {
        spdlog::info("Creating {}", config->backgroundRecordingDir.string());
        std::filesystem::create_directory(config->backgroundRecordingDir);
    }

    if (!userargs.count("no_gui")) {
        spdlog::info("Gui mode: {}", true);
        QApplication app(argc, argv);
        app.setWindowIcon(QIcon(QString("./resources/logo.png")));

        spdlog::info("Loading theme {}", "./resources/curibio.qss");
        QFile styleSheetFile("./resources/curibio.qss");
        styleSheetFile.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(styleSheetFile.readAll());
        app.setStyleSheet(styleSheet);

        MainWindow win(config);

        win.resize(1800, 1011);
        win.setVisible(true);

        if (config->asyncInit) {
            std::thread t([&] { win.Initialize(); });
            t.detach();
        } else {
            win.Initialize();
        }

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
            .workingDir = enableLongPath(config->path),
            .acquisitionDir = enableLongPath(config->path),
            .filePrefix = config->prefix,
            .region = {
                .s1 = uns16(config->rgn.s1), .s2 = uns16(config->rgn.s2), .sbin = config->rgn.sbin,
                .p1 = uns16(config->rgn.p1), .p2 = uns16(config->rgn.p2), .pbin = config->rgn.pbin
            },
            .storageType = config->storageType,
            .spdTableIdx = config->spdtable,
            .expTimeMS = static_cast<uint32_t>(config->expTimeMs),
            .trigMode = config->triggerMode,
            .expModeOut = config->exposureMode,
            .frameCount = config->frameCount,
            .bufferCount = config->bufferCount
        };

        camera->SetupExp(m_expSettings);

        std::shared_ptr<pmAcquisition> acquisition = std::make_shared<pmAcquisition>(camera);
        acquisition->StartAcquisition(nullptr, nullptr);
        acquisition->WaitForStop();
    }

    return 0;
}

#ifdef _WIN
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd) {
    return main(__argc, __argv);
}
#endif
