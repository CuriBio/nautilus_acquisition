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
 * @file  mainwindow.cpp
 *
 * @brief Implementation of the mainwindow widget.
 *********************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#define USE_IMPORT_EXPORT

#include <chrono>
#include <ctime>
#include <format>
#include <fstream>
#include <future>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <stdlib.h>
#include <thread>
#include <vector>
#include <tuple>

#ifdef _WIN64
#include <windows.h>
#include <fileapi.h>
#endif

#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <QMessageBox>
#include <QThread>
#include <QTimer>
#include <QProcess>
#include <QSvgWidget>
#include <QPushButton>
#include <QCompleter>
#include <QString>
#include <QStringListModel>

#include "mainwindow.h"

#include <PostProcess.h>
#include <RawFile.h>
#include <Database.h>
#include <processing/WriteRawFrame.h>
#include <processing/BackgroundProcess.h>
#include <Rois.h>
#include "plateidedit.h"

#define DATA_DIR "data"

std::string appStateToStr(AppState state) {
    switch (state) {
        case Uninitialized: return "Uninitialized";
        case Initializing: return "Initializing";
        case Idle: return "Idle";
        case LiveViewBtnPress: return "LiveViewBtnPress";
        case LiveViewRunning: return "LiveViewRunning";
        case AcquisitionBtnPress: return "AcquisitionBtnPress";
        case AcquisitionRunning: return "AcquisitionRunning";
        case LiveViewAcquisitionRunning: return "LiveViewAcquisitionRunning";
        case AdvSetupBtnPress: return "AdvSetupBtnPress";
        case AdvSetupOpen: return "AdvSetupOpen";
        case AdvSetupClosed: return "AdvSetupClosed";
        case SettingsBtnPress: return "SettingsBtnPress";
        case SettingsOpen: return "SettingsOpen";
        case SettingsClosed: return "SettingsClosed";
        case UserCanceled: return "UserCanceled";
        case AcquisitionDone: return "AcquisitionDone";
        case PostProcessing: return "PostProcessing";
        case PostProcessingLiveView: return "PostProcessingLiveView";
        case PostProcessingDone: return "PostProcessingDone";
        case Error: return "Error";
        default: return fmt::format("UNKNOWN STATE {}", state);
    }
}

/*
 * Instance of the main Nautilai application window.
 *
 * @param params The config params class.
 * @param parent Pointer to parent widget.
 */
MainWindow::MainWindow(std::shared_ptr<Config> params, QMainWindow *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    m_config = params;

    //show error popup
    connect(this, &MainWindow::sig_show_error, this, [this](std::string msg) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error", msg.c_str());
        messageBox.setFixedSize(500,200);
        if (!m_config->ignoreErrors) { exit(1); }
    });

    if (!m_config->configError.empty()) {
        return;
    }

    //setup width/height and initial exposure settings
    m_width = (m_config->rgn.s2 - m_config->rgn.s1 + 1) / m_config->rgn.sbin;
    m_height = (m_config->rgn.p2 - m_config->rgn.p1 + 1) / m_config->rgn.pbin;

    m_liveView = new LiveView(parent, m_width, m_height, m_config->vflip, m_config->hflip, ImageFormat::Mono16);
    m_liveView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    ui.liveViewLayout->addWidget(m_liveView);

    connect(this, &MainWindow::sig_update_state, this, &MainWindow::updateState);

    connect(this, &MainWindow::sig_disable_ui_moving_stage, this, [this]() {
        disableMask(StartAcquisitionMask);
    });

    connect(this, &MainWindow::sig_enable_ui_moving_stage, this, [this]() {
        checkStartAcqRequirements();
    });

    // set platmapFormat
    m_plateFormats = getFileNamesFromDirectory("./plate_formats");
    m_platemap = new QSvgWidget();
    ui.platemapLayout->addWidget(m_platemap, 1);

    for (size_t i = 0; i < PLATEMAP_COUNT; i++) {
        m_plateFormatImgs[i] = QString("./resources/Nautilus-software_plate-base.svg");
    }

    m_platemap->load(m_plateFormatImgs[0]);


    connect(this, &MainWindow::sig_set_platmapFormat, this, [this](QStringList qs) {
        ui.plateFormatDropDown->addItems(qs);
        ui.plateFormatDropDown->setCurrentIndex(-1);
    });

    connect(this, &MainWindow::sig_set_platemap, this, [this](size_t n) {
        m_platemap->load(m_plateFormatImgs[n]);
    });

    // plate ID widget
    m_db = new Database(m_config->userProfile);
    QCompleter *plateIdCompleter = new QCompleter(QStringList {}, this);
    ui.plateIdEdit->setCompleter(plateIdCompleter);
    updatePlateIdList();

    //settings dialog
    m_settings = new Settings(this, m_config);
    connect(m_settings, &Settings::finished, this, [this]() { emit sig_update_state(SettingsClosed); });
    connect(m_settings, &Settings::sig_settings_changed, this, &MainWindow::settingsChanged);


    //stage control
    m_stageControl = new StageControl(m_config->stageComPort, m_config, m_config->stageStepSizes, this);
    connect(m_stageControl, &StageControl::finished, this, [this]() {
        if (m_curState != AcquisitionRunning && m_curState != LiveViewAcquisitionRunning) {
            enableMask(StageNavigationMask);
        }
    });

    connect(m_stageControl, &StageControl::sig_stagelist_updated, this, [this]() {
        checkStartAcqRequirements();
    });

    connect(m_stageControl, &StageControl::sig_start_move, this, [this]() {
        emit sig_progress_start("Moving stage", 0);
        m_savedMask = m_curMask;
        setMask(DISABLE_ALL);
    });

    connect(m_stageControl, &StageControl::sig_end_move, this, [this]() {
        emit sig_progress_done();
        m_curMask = m_savedMask;
        updateInputs();
    });

    //Setup NIDAQmx controller for LED
    m_advancedSetupDialog = new AdvancedSetupDialog(m_config, this);
    connect(m_advancedSetupDialog, &AdvancedSetupDialog::sig_ni_dev_change, this, &MainWindow::setupNIDevices);
    connect(m_advancedSetupDialog, &AdvancedSetupDialog::sig_trigger_mode_change, this, &MainWindow::updateTriggerMode);
    connect(m_advancedSetupDialog, &AdvancedSetupDialog::sig_enable_live_view_during_acquisition_change, this, &MainWindow::updateEnableLiveViewDuringAcquisition);
    connect(m_advancedSetupDialog, &AdvancedSetupDialog::sig_close_adv_settings, this, [this]() { emit sig_update_state(AdvSetupClosed); });

    //fps, duration update
    m_savedDuration = m_config->duration;
    connect(this, &MainWindow::sig_set_fps_duration, this, [this](int maxfps, int fps, int duration) {
        ui.frameRateEdit->setMaximum(maxfps);
        ui.frameRateEdit->setValue((m_config->fps <= maxfps) ? m_config->fps : maxfps);
        ui.durationEdit->setValue(m_config->duration);
    });


    //progress bar
    m_acquisitionProgress = new QProgressDialog("", "Cancel", 0, 100, this, Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    m_acquisitionProgress->cancel();
    m_acquisitionProgress->setCancelButton(nullptr);
    m_acquisitionProgress->setAutoClose(false);

    connect(this, &MainWindow::sig_progress_start, this, [this](std::string msg, int n) {
        m_acquisitionProgress->setCancelButton(nullptr);
        m_acquisitionProgress->setMinimum(0);
        m_acquisitionProgress->setMaximum(n);
        m_acquisitionProgress->setValue(0);
        m_acquisitionProgress->setLabelText(QString::fromStdString(msg));
        m_acquisitionProgress->show();
    });

    connect(this, &MainWindow::sig_progress_text, this, [this](std::string msg) {
        m_acquisitionProgress->setLabelText(QString::fromStdString(msg));
        m_acquisitionProgress->setCancelButton((msg.contains("Acquiring images") && m_config->triggerMode == EXT_TRIG_TRIG_FIRST) ? new QPushButton("&Trigger", this) : nullptr);
    });

    connect(this, &MainWindow::sig_progress_update, this, [this](int n) {
        if (m_acquisitionProgress->value() + n < m_acquisitionProgress->maximum()) {
            m_acquisitionProgress->setValue(m_acquisitionProgress->value() + n);
        } else {
            m_acquisitionProgress->setValue(m_acquisitionProgress->maximum());
            m_acquisitionProgress->cancel();
        }
    }, Qt::QueuedConnection);

    connect(this, &MainWindow::sig_progress_done, this, [this]() {
        m_acquisitionProgress->setValue(m_acquisitionProgress->maximum());
        m_acquisitionProgress->cancel();
    });

    //disconnect canceled signal from all slots, specifically cancel, so that it doesn't auto close when clicked
    disconnect(m_acquisitionProgress,  &QProgressDialog::canceled, 0, 0);
    //then connect to sendManualTrigger
    connect(m_acquisitionProgress, &QProgressDialog::canceled, this, &MainWindow::sendManualTrigger);

    /*
     *  Start video encoding
     */
    connect(this, &MainWindow::sig_start_encoding, this, [&] {
        //run external video encoder command
        std::string encodingCmd = fmt::format("\"{}\" -f rawvideo -pix_fmt gray12le -r {} -s:v {}:{} -i {} -q:v {} {}",
                        m_config->ffmpegDir.string(),
                        std::to_string(m_config->fps),
                        std::to_string(m_width * m_config->cols),
                        std::to_string(m_height * m_config->rows),
                        fmt::format("\"{}_{}.raw\"", (m_expSettings.acquisitionDir / m_config->prefix).string(), std::string(m_startAcquisitionTS)),
                        std::to_string(m_config->videoQualityOptions[m_config->selectedVideoQualityOption]),
                        fmt::format("\"{}_stack_{}.avi\"", (m_expSettings.acquisitionDir / m_config->prefix).string(), std::string(m_startAcquisitionTS))
                      );

        spdlog::info("Starting video encoding {}", encodingCmd);

        m_extVidEncoder.setProcessChannelMode(QProcess::ForwardedChannels);
        m_extVidEncoder.start(QString::fromStdString(encodingCmd));
    });

    connect(&m_extVidEncoder, &QProcess::started, this, [this] { spdlog::info("Video encoding started"); });

    connect(&m_extVidEncoder, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        spdlog::info("Video encoding finished, exitCode {}, exitStatus {}", exitCode, exitStatus);
        if (m_config->enableDownsampleRawFiles && !m_config->keepOriginalRaw) {
            deleteOriginalRawFile();
        }
        emit sig_start_analysis();
    });


    connect(&m_extVidEncoder, &QProcess::errorOccurred, this, [&](QProcess::ProcessError err) {
        if (++m_extEncodingRetries < 5) {
            double backoff = m_extRetryBackoffms * std::pow(m_extEncodingRetries, 2);
            spdlog::error("Video encoding error: {}, retrying {} with backoff {}ms", err, m_extEncodingRetries, backoff);

            std::thread t([&] {
                std::this_thread::sleep_for(std::chrono::duration<double>(backoff / 1000.0)); //in seconds
                emit sig_start_encoding();
            });
            t.detach();
        } else {
            spdlog::error("Video encoding failed: {}", err);
            emit sig_start_analysis();
        }
    });

    /*
     * Start external analysis
     */
    connect(&m_extAnalysis, &QProcess::started, this, [this] {
        spdlog::info("Analysis started");
        emit sig_progress_text("Running Analysis");
    });

    connect(&m_extAnalysis, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        spdlog::info("Analysis finished, exitCode {}, exitStatus {}", exitCode, exitStatus);
        spdlog::info("------------ Analysis logs ------------\n{}", m_extAnalysis.readAllStandardOutput().toStdString());
        m_extEncodingRetries = 0;
        ui.startAcquisitionBtn->setText("Start Acquisition");

        //need to check if there is enough space for another acquisition
        checkStartAcqRequirements();
        emit sig_progress_done();
        emit sig_update_state(PostProcessingDone);
    });

    connect(&m_extAnalysis, &QProcess::errorOccurred, this, [&](QProcess::ProcessError err) {
        spdlog::error("Analysis error: {}", err);
        ui.startAcquisitionBtn->setText("Start Acquisition");

        //need to check if there is enough space for another acquisition
        checkStartAcqRequirements();
        emit sig_progress_done();
        emit sig_update_state(PostProcessingDone);
    });

    connect(this, &MainWindow::sig_start_analysis, this, [&] {
        //run external analysis, probably want to start another progress bar/spinner
        std::filesystem::path settingsPath = m_expSettings.acquisitionDir / "settings.toml";
        spdlog::info("Starting analysis {} with {}", m_config->extAnalysis.string(), settingsPath.string());

        m_extAnalysis.setProcessChannelMode(QProcess::SeparateChannels);
        m_extAnalysis.start(QString::fromStdString(m_config->extAnalysis.string()), QStringList() << settingsPath.string().c_str());
    });


    m_expSettings.workingDir = enableLongPath(m_config->path);
    m_expSettings.acquisitionDir = enableLongPath(m_config->path);
    m_expSettings.filePrefix = m_config->prefix;
    m_expSettings.spdTableIdx = m_config->spdtable;
    m_expSettings.expTimeMS = m_config->expTimeMs,
    m_expSettings.frameCount = m_config->frameCount;
    m_expSettings.bufferCount = m_config->bufferCount;
    m_expSettings.storageType = m_config->storageType;
    m_expSettings.trigMode = m_config->triggerMode;
    m_expSettings.expModeOut = m_config->exposureMode;
    m_expSettings.region = {
        .s1 = uns16(m_config->rgn.s1), .s2 = uns16(m_config->rgn.s2), .sbin = m_config->rgn.sbin,
        .p1 = uns16(m_config->rgn.p1), .p2 = uns16(m_config->rgn.p2), .pbin = m_config->rgn.pbin
    };

    spdlog::info("Setting region: (s1: {}, s2: {}, p1: {}, p2: {}, sbin: {}, pbin: {}",
        m_expSettings.region.s1, m_expSettings.region.s2,
        m_expSettings.region.p1, m_expSettings.region.p2,
        m_expSettings.region.sbin, m_expSettings.region.pbin
    );
    spdlog::info("Image capture width: {}, height: {}", m_width, m_height);

    //initial autoupdate class
    m_autoUpdate = new AutoUpdate(
        m_config,
        //need to use this instead of downloads.curibio.com b/c cloudfront caches files for 24 hours
        "https://s3.amazonaws.com/downloads.curibio.com/software/nautilai",
        "prod",
        this
    );

    connect(m_autoUpdate, &AutoUpdate::sig_update_accepted, this, [this] {
        emit sig_progress_start("Applying update", 0);
        m_autoUpdate->applyUpdate();
        emit sig_progress_done();

        m_config->updateAvailable = false;
        close();
    });

    connect(m_autoUpdate, &AutoUpdate::sig_update_ignored, this, [this] {
        m_config->updateAvailable = false;
        close();
    });


    //live view timer signals
    m_liveViewTimer = new QTimer(this);
    connect(m_liveViewTimer, &QTimer::timeout, this, &MainWindow::updateLiveView);

    //initialize histogram buffer
    m_hist = new uint32_t[(1<<16) - 1];
    memset((void*)m_hist, 0, sizeof(uint32_t)*((1<<16)-1));

    //initialize lut buffer
    m_lut16 = new uint8_t[(1<<16) - 1];
    memset((void*)m_lut16, 0, (1<<16)-1);

    //create task pools
    m_taskFrameStats = std::make_shared<TaskFrameStats>(TASKS);
    emit sig_update_state(Initializing);
}

/*
 * Initializes main window and camera/acquisition objects.
 */
void MainWindow::Initialize() {
    spdlog::info("Checking for config errors");
    if (!m_config->configError.empty()) {
        emit sig_show_error(m_config->configError);
        return;
    }
    //set options for plate formats drop down
    emit sig_set_platmapFormat(vectorToQStringList(m_plateFormats));
    emit sig_progress_start("Initializing Camera", 0);

    spdlog::info("Initialize camera");
    m_camera = std::make_shared<pmCamera>();

    spdlog::info("Opening camera 0");
    if (!m_camera->Open(0)) {
        spdlog::error("Failed to open camera 0");
        emit sig_show_error("Camera could not be found, please plug in camera and restart application");
        return;
    }

    m_camInfo = m_camera->GetInfo();
    m_camera->SetupExp(m_expSettings);

    emit sig_progress_text("Calibrating stage");

    //Async calibrate stage
    if (m_config->asyncInit) {
        m_stageCalibrate = std::async(std::launch::async, [&] {
            return m_stageControl->Calibrate();
        });

        m_niSetup = std::async(std::launch::async, [&] {
            setupNIDevices(m_config->niDev, m_config->trigDev);
            m_advancedSetupDialog->Initialize(m_DAQmx.GetListOfDevices());
        });

        m_autoUpdateCheck = std::async(std::launch::async, [&] {
            m_autoUpdate->hasUpdate();
            spdlog::info("Update available {}", m_config->updateAvailable);
        });
    } else {
        m_stageControl->Calibrate();
        emit sig_progress_done();

        //setup NI device
        setupNIDevices(m_config->niDev, m_config->trigDev);
        m_advancedSetupDialog->Initialize(m_DAQmx.GetListOfDevices());

        //check for update
        m_autoUpdate->hasUpdate();
        spdlog::info("Update available {}", m_config->updateAvailable);
    }

    //for 8 bit image conversion for liveview, might not need it anymore
    m_img16 = new uint16_t[m_width*m_height];

    //Set sensor size for live view
    //ui.liveView->Init(m_width, m_height, m_config->vflip, m_config->hflip, ImageFormat::Mono16);
    ui.histView->Init(m_hist, m_width*m_height);

    //log speed table
    spdlog::info("Speed Table:");
    for(auto& i : m_camInfo.spdTable) {
        spdlog::info("\tport: {}, pixTimeNs: {}, spdIndex: {}, gainIndex: {}, gainName: {}, bitDepth: {}", i.portName, i.pixTimeNs, i.spdIndex, i.gainIndex, i.gainName, i.bitDepth);
    }

    //needs camera to be opened first
    m_acquisition = std::make_unique<pmAcquisition>(m_camera);
    if (m_config->testImgPath != "") {
        m_acquisition->LoadTestData(m_config->testImgPath);
    }


    ui.ledIntensityEdit->setValue(m_config->ledIntensity);

    //Get max Frame rate
    double max_fps = 1000000.0 / double(m_config->lineTimes[m_expSettings.spdTableIdx] * abs(m_expSettings.region.p2 - m_expSettings.region.p1));
    emit sig_set_fps_duration(max_fps, (m_config->fps <= max_fps) ? m_config->fps : max_fps, m_config->duration);
    spdlog::info("Max frame rate: {}", max_fps);

    //Wait for stage calibration
    if (m_config->asyncInit) {
        m_stageCalibrate.wait();
        if (!m_stageCalibrate.get()) {
            spdlog::error("Stage calibration failed");
        }
    }

    //wait for ni device setup
    if (m_config->asyncInit) {
        m_niSetup.wait();
    }

    // Add data type options
    ui.dataTypeList->addItem(QString("Calcium Imaging"));
    ui.dataTypeList->addItem(QString("Voltage Imaging"));
    ui.dataTypeList->addItem(QString("Background Recording"));
    ui.dataTypeList->setCurrentIndex(0);

    emit sig_progress_done();
    emit sig_update_state(Idle);
}

void MainWindow::updateState(AppState state) {
    auto fn = m_appTransitions[{m_curState, state}];
    if (fn) {
        spdlog::info("Update state {} -> {}", appStateToStr(m_curState), appStateToStr(state));
        fn();
        spdlog::info("Update state {} -> {}", appStateToStr(state), appStateToStr(m_curState));
    } else {
        spdlog::error("Invalid state transition: {} -> {}", appStateToStr(m_curState), appStateToStr(state));
    }
}

void MainWindow::updateInputs() {
    //probably should be moved, but when the QComboBox is enabled after being disabled
    //it resets the index to 0, which is not what we want on startup
    ui.plateFormatDropDown->setEnabled(testMask(PlateMapMask));
    if (testMask(PlateMapMask)) {
        ui.plateFormatDropDown->setCurrentIndex(m_plateFormatCurrentIndex);
    } else {
        m_plateFormatCurrentIndex = ui.plateFormatDropDown->currentIndex();
    }

    ui.durationEdit->setEnabled(testMask(DurationMask));
    ui.frameRateEdit->setEnabled(testMask(FrameRateMask));
    ui.ledIntensityEdit->setEnabled(testMask(LedIntensityMask));
    ui.startAcquisitionBtn->setEnabled(testMask(StartAcquisitionMask));
    ui.liveScanBtn->setEnabled(testMask(LiveScanMask));
    ui.advancedSetupBtn->setEnabled(testMask(AdvancedSetupMask));
    ui.settingsBtn->setEnabled(testMask(SettingsMask));
    ui.stageNavigationBtn->setEnabled(testMask(StageNavigationMask) && !m_stageControl->isVisible());
    ui.disableBackgroundRecording->setEnabled(testMask(DisableBackgroundRecordingMask));

    if (!testMask(StageNavigationMask)) {
        emit m_stageControl->sig_stage_disable_all();
    } else {
        emit m_stageControl->sig_stage_enable_all();
    }
}


//state handlers
bool MainWindow::startLiveView() {
    spdlog::info("Starting liveview");
    //emit sig_disable_all();

    setMask(LiveScanMask | LedIntensityMask | StageNavigationMask);
    checkStartAcqRequirements();
    emit m_stageControl->sig_stage_enable_all();

    double voltage = (m_config->ledIntensity / 100.0) * m_config->maxVoltage;
    ledON(voltage, false);

    // max frame rate allowed in live scan is 24, acquisition can capture at higher frame rates
    double minFps = std::min<double>(m_config->fps, 24.0);
    m_liveViewTimer->start(int32_t(1000 * (1.0 / minFps)));
    ui.liveScanBtn->setText("Stop Live Scan");

    double expTimeMs = (1.0 / m_config->fps) * 1000;
    spdlog::info("Setting expTimeMS: {} ({})", static_cast<uint32_t>(expTimeMs), expTimeMs);

    m_expSettings.expTimeMS = static_cast<uint32_t>(expTimeMs);
    m_expSettings.frameCount = uint32_t(m_config->duration * m_config->fps);

    //have to set to this camera mode for live view, user might be using external ttl trigger
    //which won't start capture until the ttl signal.
    m_expSettings.trigMode = EXT_TRIG_INTERNAL;

    m_camera->UpdateExp(m_expSettings);
    spdlog::info("Starting live view: expTimeMS {}", m_expSettings.expTimeMS);

    if (!m_acquisition) {
        spdlog::info("Creating acquisition");
        m_acquisition = std::make_unique<pmAcquisition>(m_camera);
    }
    m_acquisition->StartLiveView();

    return true;
}

bool MainWindow::startLiveView_PostProcessing() {
    spdlog::info("Starting liveview post processing");
    setMask(LiveScanMask);

    emit m_stageControl->sig_stage_enable_all();

    double voltage = (m_config->ledIntensity / 100.0) * m_config->maxVoltage;
    ledON(voltage, false);

    // max frame rate allowed in live scan is 24, acquisition can capture at higher frame rates
    double minFps = std::min<double>(m_config->fps, 24.0);
    m_liveViewTimer->start(int32_t(1000 * (1.0 / minFps)));
    ui.liveScanBtn->setText("Stop Live Scan");

    double expTimeMs = (1.0 / m_config->fps) * 1000;
    spdlog::info("Setting expTimeMS: {} ({})", static_cast<uint32_t>(expTimeMs), expTimeMs);

    m_expSettings.expTimeMS = static_cast<uint32_t>(expTimeMs);
    m_expSettings.frameCount = uint32_t(m_config->duration * m_config->fps);

    //have to set to this camera mode for live view, user might be using external ttl trigger
    //which won't start capture until the ttl signal.
    m_expSettings.trigMode = EXT_TRIG_INTERNAL;

    m_camera->UpdateExp(m_expSettings);
    spdlog::info("Starting live view: expTimeMS {}", m_expSettings.expTimeMS);

    if (!m_acquisition) {
        spdlog::info("Creating acquisition");
        m_acquisition = std::make_unique<pmAcquisition>(m_camera);
    }
    m_acquisition->StartLiveView();
    return true;

}

bool MainWindow::stopLiveView() {
    spdlog::info("Stop liveview");
    setMask(ENABLE_ALL);
    checkStartAcqRequirements();
    ui.liveScanBtn->setText("Live Scan");
    ledOFF();

    m_liveViewTimer->stop();
    m_liveView->update();
    m_acquisition->StopAll();
    m_acquisition->WaitForStop();

    return true;
}

bool MainWindow::stopLiveView_PostProcessing() {
    spdlog::info("Stop liveview post processing");
    emit m_stageControl->sig_stage_enable_all();
    ui.liveScanBtn->setText("Live Scan");
    ledOFF();

    m_liveViewTimer->stop();
    m_liveView->update();
    m_acquisition->StopAll();
    m_acquisition->WaitForStop();

    return true;

}

bool MainWindow::startAcquisition() {
    if (m_plateFormatCurrentIndex == -1) {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Warning!");
        messageBox.setText("Automatic analysis will not run because no plate format has been selected for this acquisition.\n"
            "Click cancel and select the plate format to enable automatic analysis."
            " To continue the acquisition with automatic analysis disabled, click continue.");
        messageBox.setIcon(QMessageBox::NoIcon);
        messageBox.addButton(QMessageBox::Cancel);
        messageBox.addButton(QString("Continue"), QMessageBox::NoRole);

        m_userCanceledAcquisition = messageBox.exec() == QMessageBox::Cancel;
        if (m_userCanceledAcquisition) {
            spdlog::info("User canceled acquisition start because no platemap was selected.");
            return false;
        } else {
            spdlog::info("User selected to continue acquisition with no platemap selected.");
        }
    }

    spdlog::info("Starting acquisition");

    if (ui.dataTypeList->currentText().toStdString() == "Background Recording") {
        m_acquisitionThread = QThread::create(MainWindow::backgroundRecordingThread, this);
    } else {
        m_acquisitionThread = QThread::create(MainWindow::acquisitionThread, this);
    }

    connect(m_acquisitionThread, &QThread::finished, m_acquisitionThread, [this]() {
        &QThread::quit;
        delete m_acquisitionThread;
        m_acquisitionThread = nullptr;
    });

    m_userCanceled = false;
    setMask((m_config->enableLiveViewDuringAcquisition ? LiveScanMask : 0) | StartAcquisitionMask | LedIntensityMask);

    ui.startAcquisitionBtn->setText("Stop Acquisition");
    m_acquisitionThread->start();

    return true;
}

bool MainWindow::stopAcquisition() {
    spdlog::info("Stopping acquisition");
    setMask(ENABLE_ALL);
    checkStartAcqRequirements();
    emit m_stageControl->sig_stage_enable_all();
    emit sig_progress_done();

    ui.startAcquisitionBtn->setText("Start Acquisition");
    ledOFF();

    m_acquisition->StopAll();
    m_acquisition->WaitForStop();
    m_userCanceled = true;

    return true;
}

bool MainWindow::startLiveView_AcquisitionRunning() {
    spdlog::info("Live view + Acquisition starting");
    ui.liveScanBtn->setText("Stop Live Scan");

    // max frame rate allowed in live scan is 24, acquisition can capture at higher frame rates
    double minFps = std::min<double>(m_config->fps, 24.0);
    m_liveViewTimer->start(int32_t(1000 * (1.0 / minFps)));
    m_acquisition->StartLiveView();

    return true;
}

bool MainWindow::startAcquisition_LiveViewRunning() {
    spdlog::info("Live view + Acquisition starting");
    emit m_stageControl->sig_stage_disable_all();
    ui.startAcquisitionBtn->setText("Stop Acquisition");
    disableMask(StageNavigationMask);

    m_userCanceled = false;

    if (ui.dataTypeList->currentText().toStdString() == "Background Recording") {
        m_acquisitionThread = QThread::create(MainWindow::backgroundRecordingThread, this);
    } else {
        m_acquisitionThread = QThread::create(MainWindow::acquisitionThread, this);
    }

    connect(m_acquisitionThread, &QThread::finished, m_acquisitionThread, [this]() {
        &QThread::quit;
        delete m_acquisitionThread;
        m_acquisitionThread = nullptr;
    });
    m_acquisitionThread->start();

    return true;
}

bool MainWindow::stopAcquisition_LiveViewRunning() {
    spdlog::info("Stopping Acquisition, Live view still running");
    ui.startAcquisitionBtn->setText("Start Acquisition");
    enableMask(StageNavigationMask | LiveScanMask);
    emit m_stageControl->sig_stage_enable_all();

    m_acquisition->StopCapture();
    m_userCanceled = true;
    emit sig_progress_done();

    return true;
}

bool MainWindow::stopLiveView_AcquisitionRunning() {
    spdlog::info("Stopping Live view, acquisition still running");
    ui.liveScanBtn->setText("Live Scan");
    m_liveViewTimer->stop();
    m_liveView->update();

    return true;

}

bool MainWindow::advSetupOpen() {
    spdlog::info("Opening Advanced Setup Dialog");
    setMask(DISABLE_ALL);
    m_advancedSetupDialog->show();
    return true;
}

bool MainWindow::advSetupClosed() {
    spdlog::info("Advanced Setup Closed");
    setMask(ENABLE_ALL);
    checkStartAcqRequirements();
    return true;
}

bool MainWindow::settingsOpen() {
    spdlog::info("Open Settings Dialog");
    setMask(DISABLE_ALL);
    m_settings->show();
    return true;
}

bool MainWindow::settingsClosed() {
    spdlog::info("Settings Closed");
    setMask(ENABLE_ALL);
    checkStartAcqRequirements();
    return true;
}

bool MainWindow::startPostProcessing() {
    spdlog::info("Start PostProcessing");
    disableMask(StartAcquisitionMask);
    m_acquisition->StopAll();

    std::thread postProcessThread([this]() {
        spdlog::info("Starting post processing thread");
        ledOFF();
        m_acquisition->WaitForStop();

        postProcess();

        m_userCanceled = false;

        std::thread deleteT([this]() {
            spdlog::info("Deleting files");
            std::uintmax_t n = std::filesystem::remove_all(m_expSettings.acquisitionDir / DATA_DIR);
            spdlog::info("Deleted {} files", n);
        });
        deleteT.detach();

        if (m_config->encodeVideo) {
            emit sig_progress_start("Encoding Video", 0);
            emit sig_start_encoding();
        } else {
            if (m_config->enableDownsampleRawFiles && !m_config->keepOriginalRaw) {
                deleteOriginalRawFile();
            }

            emit sig_update_state(PostProcessingDone);
        }
    });

    postProcessThread.detach();
    return true;
}

bool MainWindow::startPostProcessing_LiveViewRunning() {
    spdlog::info("Stopping Live View to Start Post Processing");
    stopLiveView();
    startPostProcessing();

    return true;
}

void MainWindow::deleteOriginalRawFile() {
    std::string rawFile = fmt::format("{}_{}.raw", (m_expSettings.acquisitionDir / m_config->prefix).string(), std::string(m_startAcquisitionTS));
    bool wasDeleted = std::filesystem::remove(rawFile);
    if (wasDeleted) {
        spdlog::info("Successfully deleted {}", rawFile);
    } else {
        spdlog::info("Failed to delete {}", rawFile);
    }
}

bool MainWindow::postProcessingDone() {
    spdlog::info("Post Processing Done");

    setMask(ENABLE_ALL);
    checkStartAcqRequirements();
    emit m_stageControl->sig_stage_enable_all();
    return true;
}

bool MainWindow::postProcessingDone_LiveViewRunning() {
    spdlog::info("Post Processing Done + Live View Running");
    setMask(LiveScanMask | StartAcquisitionMask | LedIntensityMask | (m_stageControl->isVisible() ? 0 : StageNavigationMask));
    emit m_stageControl->sig_stage_enable_all();

    return true;
}

void MainWindow::on_levelsSlider_valueChanged(int value) {
    ui.levelMax->setText(QString::number(value));
    m_liveView->SetLevel(value);
    m_liveView->update();
}

/*
 * Frame Rate edit box slot, called when users changes the FPS value.
 *
 * @param value The updated FPS value.
 */
void MainWindow::on_frameRateEdit_valueChanged(double value) {
    m_config->fps = value;
    if (m_curState == LiveViewRunning) {
        m_liveViewTimer->stop();
        double minFps = std::min<double>(m_config->fps, 24.0);
        m_liveViewTimer->start(int32_t(1000 * (1.0 / minFps)));
    }
    checkStartAcqRequirements();
}


void MainWindow::on_dataTypeList_currentTextChanged(const QString &text) {
    if (text.toStdString() == "Background Recording") {
        disableMask(DisableBackgroundRecordingMask | SettingsMask | AdvancedSetupMask | DurationMask | StageNavigationMask);

        //save current duration so it can be set back when data type is changed
        m_savedDuration = m_config->duration;
        ui.durationEdit->setValue(1.0);
        ui.durationEdit->setEnabled(false);

        //disable use background recording checkbox
        ui.disableBackgroundRecording->setChecked(false);

        m_config->recordingType = RecordingType::Background;
    } else {
        enableMask(DisableBackgroundRecordingMask | SettingsMask | AdvancedSetupMask | DurationMask | StageNavigationMask);

        m_config->duration = m_savedDuration;
        ui.durationEdit->setValue(m_config->duration);
        ui.durationEdit->setEnabled(true);

        ui.disableBackgroundRecording->setChecked(false);

        m_config->recordingType = (text.toStdString() == "Calcium") ? RecordingType::Calcium : RecordingType::Voltage;
    }

    checkStartAcqRequirements();
}

void MainWindow::on_plateFormatDropDown_activated(int index) {
    m_config->plateFormat = m_plateFormats[index];
    m_plateFormatCurrentIndex = index;
    auto plateFormatFileName = m_config->plateFormat.string();

    m_stageControl->loadList(plateFormatFileName);
    spdlog::info("Setting platemap for plate format {}", m_plateFormats[index].string());

    try {
        auto plateFormatFile = toml::parse(plateFormatFileName);
        auto numWells = toml::find<int>(plateFormatFile, "stage", "num_wells");

        if (numWells == 24) {
            m_plateFormatImgs[0] = QString("./resources/Nautilus-software_24-well-plate-inactive.svg");
            for (size_t i = 1; i < PLATEMAP_COUNT; i++) {
                m_plateFormatImgs[i] = QString::fromStdString(fmt::format("./resources/Nautilus-software_24-well-plate-section{}-active.svg", i));
            }
        } else if (numWells == 96) {
            m_plateFormatImgs[0] = QString::fromStdString("./resources/Nautilus-software_96-well-plate-round-inactive.svg");
            for (size_t i = 1; i < PLATEMAP_COUNT; i++) {
                m_plateFormatImgs[i] = QString::fromStdString(fmt::format("./resources/Nautilus-software_96-well-plate-round-section{}-active.svg", i));
            }
        } else {
            spdlog::error(fmt::format("No platemap svg for {} well plate", numWells));
            for (size_t i = 0; i < PLATEMAP_COUNT; i++) {
                m_plateFormatImgs[i] = QString("./resources/Nautilus-software_plate-base.svg");
            }
        }

        m_platemap->load(m_plateFormatImgs[0]);

        m_roiCfg.well_spacing = toml::find<uint32_t>(plateFormatFile, "stage", "well_spacing");
        m_roiCfg.xy_pixel_size = m_config->xyPixelSize;
        m_roiCfg.scale = m_config->rgn.sbin;
        m_roiCfg.rows = toml::find<uint32_t>(plateFormatFile, "stage", "num_wells_v");
        m_roiCfg.cols = toml::find<uint32_t>(plateFormatFile, "stage", "num_wells_h");
        m_roiCfg.width = toml::find<uint32_t>(plateFormatFile, "stage", "roi_size_x");
        m_roiCfg.height = toml::find<uint32_t>(plateFormatFile, "stage", "roi_size_y");
        m_roiCfg.v_offset = toml::find<int32_t>(plateFormatFile, "stage", "v_offset");
        m_roiCfg.h_offset = toml::find<int32_t>(plateFormatFile, "stage", "h_offset");

        std::vector<std::tuple<uint32_t, uint32_t>> rois = Rois::roiOffsets(&m_roiCfg, m_width, m_height);
        m_liveView->UpdateRois(&m_roiCfg, rois);

    } catch(const std::exception &e) {
        spdlog::error("Failed to load platemap format values, {}", e.what());
    }
}


/*
 * Duration edit box slot, called when users changes duration value.
 *
 * @param value The updated duration value in seconds.
 */
void MainWindow::on_durationEdit_valueChanged(double value) {
    m_config->duration = value;
    checkStartAcqRequirements();
}

void MainWindow::on_plateIdEdit_textChanged(const QString &plateId) {
    m_config->plateId = plateId.toStdString();
    if (m_config->plateId == "" ) {
        // if empty, need to use unfiltered completion so that all options are included.
        // other popup completion will not show all options if no text entered
        ui.plateIdEdit->completer()->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    } else {
        // if not empty, only want to match options based on the prefix
        ui.plateIdEdit->completer()->setCompletionMode(QCompleter::PopupCompletion);
    }
    checkStartAcqRequirements();
}

void MainWindow::on_plateIdEdit_editingFinished() {
    spdlog::info("Set plateId: '{}'", m_config->plateId);
}

void MainWindow::on_disableBackgroundRecording_stateChanged(int state) {
    m_config->useBackgroundSubtraction = state == 0;
    if (m_config->useBackgroundSubtraction) {
        spdlog::info("Background subtraction enabled");
    } else {
        spdlog::info("Background subtraction disabled");
    }
    checkStartAcqRequirements();
}

void MainWindow::checkStartAcqRequirements() {
    // these will handle setting the tooltip of the start acq btn for an error, so order matters here to get the most important errors to show up over others
    bool validPlateIdVals = checkPlateIdRequirements();
    bool validFrameRateAndDur = checkFrameRateAndDur();
    bool isAvailableDriveSpace = availableDriveSpace();
    if (validPlateIdVals && validFrameRateAndDur && isAvailableDriveSpace) {
        ui.startAcquisitionBtn->setToolTip("");
        enableMask(StartAcquisitionMask);
    } else {
        disableMask(StartAcquisitionMask);
    }
    updateInputs();
}

void MainWindow::updatePlateIdList() {
    QStringList newList;
    m_config->storedPlateIds = m_db->getPlateIds();

    for (auto pid : m_config->storedPlateIds) {
        newList << QString::fromStdString(pid);
    }

    QStringListModel* model = (QStringListModel*)(ui.plateIdEdit->completer()->model());
    model->setStringList(newList);
}

void MainWindow::saveBackgroundRecordingMetadata() {
    std::string plateFormat = ui.plateFormatDropDown->currentText().toStdString();

    if (plateFormat != "") {
        m_config->plateId = ui.plateIdEdit->text().toStdString();
        QStringListModel* model = (QStringListModel*)(ui.plateIdEdit->completer()->model());

        if (model->stringList().contains(QString::fromStdString(m_config->plateId))) {
            m_db->overwritePlateId(m_config->plateId, plateFormat);
        } else {
            std::string filePath = (m_config->backgroundRecordingDir / m_config->plateId).string();
            m_db->addPlateId(m_config->plateId, plateFormat, filePath);
        }

        // update list after updating DB
        updatePlateIdList();
    }
}


/*
 * Signal to indicate the user has modified the settings.
 *
 * @param path The path to save captured images to.
 * @param prefix The file prefix to use for captured images.
 */
void MainWindow::settingsChanged(std::filesystem::path path, std::string prefix) {
    spdlog::info("Settings changed, dir: {}, prefix: {}", path.string().c_str(), prefix);
    m_config->path = path;
    m_config->prefix = prefix;

    m_expSettings.workingDir = enableLongPath(m_config->path);
    m_expSettings.acquisitionDir = enableLongPath(m_config->path);
    m_expSettings.filePrefix = m_config->prefix;
    m_camera->UpdateExp(m_expSettings);
}


/*
* Runs when a new ni device is selected, re configure ni device leds
*/
void MainWindow::setupNIDevices(std::string niDev, std::string trigDev) {
    m_config->niDev = niDev;
    m_config->trigDev = trigDev;

    //Setup NIDAQmx controller for LED
    m_ledTaskAO = "Analog_Out_Volts"; //Task for setting Analog Output voltage
    m_ledDevAO = fmt::format("{}/ao0", m_config->niDev); //Device name for analog output
    spdlog::info("Using NI device {} for led analog output", m_ledDevAO);
    m_DAQmx.ClearTask(m_ledTaskAO);

    m_ledTaskDO = "Digital_Out"; //Task for setting Digital Output
    m_ledDevDO = fmt::format("{}/port0/line0:7", m_config->niDev); //Device for digital output
    spdlog::info("Using NI device {} for led digital output", m_ledDevDO);
    m_DAQmx.ClearTask(m_ledTaskDO);

    //Setup NIDAQmx controller for manual trigger
    m_trigTaskDO = "Trigger_Digital_Out";
    m_trigDevDO = fmt::format("{}/port0/line0:7", trigDev);
    spdlog::info("Using NI device {} for trigger digital output", m_trigDevDO);
    m_DAQmx.ClearTask(m_trigTaskDO);

    m_DAQmx.CreateTask(m_ledTaskAO);
    m_DAQmx.CreateTask(m_ledTaskDO);
    m_DAQmx.CreateTask(m_trigTaskDO);

    m_DAQmx.CreateAnalogOutpuVoltageChan(m_ledTaskAO, m_ledDevAO.c_str(), -10.0, 10.0, DAQmx_Val_Volts);
    m_DAQmx.CreateDigitalOutputChan(m_ledTaskDO, m_ledDevDO.c_str(), DAQmx_Val_ChanForAllLines);
    m_DAQmx.CreateDigitalOutputChan(m_trigTaskDO, m_trigDevDO.c_str(), DAQmx_Val_ChanForAllLines);
}


/*
* Runs when a new trigger mode is selected, re configure exposure settings of camera
*/
void MainWindow::updateTriggerMode(int16_t triggerMode) {
    spdlog::info("trigger mode updated: {}", triggerMode);

    m_config->triggerMode = triggerMode;
    m_expSettings.trigMode = triggerMode;
    m_camera->UpdateExp(m_expSettings);
}


/*
* Runs when state is changed for this value
*/
void MainWindow::updateEnableLiveViewDuringAcquisition(bool enable) {
    spdlog::info("enable live view during acquisition updated: {}", enable);

    m_config->enableLiveViewDuringAcquisition = enable;
}


bool MainWindow::checkFrameRateAndDur() {
    bool invalid = m_config->duration * m_config->fps < 1.0;
    if (invalid) {
        spdlog::error("Capture is set to less than 1 frame, fps: {}, duration: {}", m_config->fps, m_config->duration);
        ui.frameRateEdit->setStyleSheet("background-color: red");
        ui.frameRateEdit->setToolTip("Capture is set to less than 1 frame");
        ui.durationEdit->setStyleSheet("background-color: red");
        ui.durationEdit->setToolTip("Capture is set to less than 1 frame");
        ui.startAcquisitionBtn->setToolTip("Capture is set to less than 1 frame");
    } else {
        ui.durationEdit->setStyleSheet("background-color: #2F2F2F");
        ui.durationEdit->setToolTip("");
        // frame rates <= 1Hz are discouraged but not disallowed
        if (m_config->fps <= 1.0) {
            spdlog::error("Frame rate is set to <= 1Hz");
            ui.frameRateEdit->setStyleSheet("background-color: red"); // TODO if ok with changing border instead of background, use yellow instead
            ui.frameRateEdit->setToolTip("Warning: frame rates <= 1Hz will likely result in poor signal to noise ratios");
        } else {
            ui.frameRateEdit->setStyleSheet("background-color: #2F2F2F");
            ui.frameRateEdit->setToolTip("");
        }
    }

    m_expSettings.expTimeMS = (1 / m_config->fps) * 1000;
    m_expSettings.frameCount = m_config->duration * m_config->fps;

    spdlog::info("Setting new exposure value, fps: {}, frame count: {}, exposure time ms: {}", m_config->fps, m_expSettings.frameCount, m_expSettings.expTimeMS);

    return !invalid;
}


bool MainWindow::checkPlateIdRequirements() {
    std::string startAcqBtnTooltip = "";

    std::string plateIdEditStyling = "background-color: #2F2F2F";

    if (m_config->recordingType == RecordingType::Background) {
        if (!ui.plateIdEdit->isEnabled()) {
            ui.plateIdEdit->setEnabled(true);
            ui.plateIdEdit->setText("");    
        }
        // a background recording must be given a plate ID before acquisition can begin
        if (m_config->plateId == "") {
            startAcqBtnTooltip = "Plate ID required for background recording";
            plateIdEditStyling = "background-color: red";
        } else {
            plateIdEditStyling = "background-color: #2F2F2F"; // TODO use green if changing border color is ok
        }
    } else if (m_config->useBackgroundSubtraction) {
        if (!ui.plateIdEdit->isEnabled()) {
            ui.plateIdEdit->setEnabled(true);
            ui.plateIdEdit->setText("");    
        }
        // if using background subtraction, a valid plate ID must be entered before starting acquisition
        bool plateIdExists = std::find(m_config->storedPlateIds.begin(), m_config->storedPlateIds.end(), m_config->plateId) != m_config->storedPlateIds.end();
        if (!plateIdExists) {
            startAcqBtnTooltip = "Invalid Plate ID";
            plateIdEditStyling = "background-color: red";
        } else {
            plateIdEditStyling = "background-color: #2F2F2F"; // TODO use green if changing border color is ok
        }
    } else {
        ui.plateIdEdit->setEnabled(false);
        // if set to empty the placeholder value will show, using a space so that it does not
        ui.plateIdEdit->setText(" ");
    }

    ui.plateIdEdit->setStyleSheet(QString::fromStdString(plateIdEditStyling));

    bool valid = startAcqBtnTooltip == "";
    if (!valid) {
        ui.startAcquisitionBtn->setToolTip(QString::fromStdString(startAcqBtnTooltip));
    }

    return valid;
}


/*
 * Turns on LED with given voltage.
 *
 * @param voltage The value to set analog output voltage to.
 *
 * @return True if successful, false otherwise.
 */
bool MainWindow::ledON(double voltage, bool delay) {
    const double data[1] = { voltage };
    uint8_t lines[8] = {1,1,1,1,1,1,1,1};
    bool rtnval = true;

    spdlog::info("m_led: {}", m_led);
    if (!m_led) {
        if (!ledSetVoltage(voltage)) {
            spdlog::error("Failed to run taskAO");
        }

       bool taskDO_result = (
            m_DAQmx.StartTask(m_ledTaskDO) && \
            m_DAQmx.WriteDigitalLines(m_ledTaskDO, 1, 0, 10.0, DAQmx_Val_GroupByChannel, lines, NULL) && \
            m_DAQmx.StopTask(m_ledTaskDO)
        );

        if (!taskDO_result) {
            spdlog::error("Failed to run taskDO");
            m_DAQmx.StopTask(m_ledTaskDO);
            rtnval = false;
        }

        if (delay) {
            spdlog::info("led ON, delaying {}ms", m_config->shutterDelayMs);
            std::this_thread::sleep_for(std::chrono::milliseconds(m_config->shutterDelayMs));
        }

        m_led = true;
    }
    return rtnval;
}


/*
 * Turns off LED.
 *
 * @return true if successful, false otherwise.
 */
bool MainWindow::ledOFF() {
    if (m_led) {
        spdlog::info("led OFF");
        uint8_t lines[8] = {0,0,0,0,0,0,0,0};
        bool taskDO_result = (
            m_DAQmx.StartTask(m_ledTaskDO) && \
            m_DAQmx.WriteDigitalLines(m_ledTaskDO, 1, 0, 10.0, DAQmx_Val_GroupByChannel, lines, NULL) && \
            m_DAQmx.StopTask(m_ledTaskDO)
        );

        if (!taskDO_result) {
            spdlog::error("Failed to run taskDO");
            return m_DAQmx.StopTask(m_ledTaskDO);
        }
        m_led = false;
    }
    return true;
}


/*
 * Sets analog output voltage for LED controller.
 *
 * @param voltage The voltage value to set on analog output channel.
 *
 * @return true is successufl, false otherwise.
 */
bool MainWindow::ledSetVoltage(double voltage) {
    const double data[1] = { voltage };
    return (
        m_DAQmx.StartTask(m_ledTaskAO) && \
        m_DAQmx.WriteAnalogF64(m_ledTaskAO, 1, 0, 10.0, DAQmx_Val_GroupByChannel, data, NULL) && \
        m_DAQmx.StopTask(m_ledTaskAO)
    );
}


/*
 * Check if default drive on windows has sufficient space for acquisition.
 * Update startAcquisitionBtn if error.
 *
 * @returns boolean true if space is available
*/
bool MainWindow::availableDriveSpace() {
    double fps = m_config->fps;
    double duration = m_config->duration;
    size_t nStagePositions = m_stageControl->GetPositions().size();
#ifdef _WIN32
    if (m_camera->ctx) {
        uns32 frameBytes = m_camera->ctx->frameBytes;
        //account for each acquisition and if autotile is enabled
        uint32_t totalAcquisitionBytes = nStagePositions * fps * duration * frameBytes * (m_config->autoTile) ? 2 : 1;

        ULARGE_INTEGER  lpTotalNumberOfFreeBytes = {0};
        std::stringstream tool_tip_text;

        if (!GetDiskFreeSpaceEx(m_config->path.c_str(), nullptr, nullptr, &lpTotalNumberOfFreeBytes)) {
            //default drive could not be found
            spdlog::error("Default drive could not be found when");
            ui.startAcquisitionBtn->setToolTip(QString::fromStdString(fmt::format("Drive {} not found", m_config->path.string())));
            return false;
        }

       if (lpTotalNumberOfFreeBytes.QuadPart > totalAcquisitionBytes) {
            std::stringstream storage_space_string;
            storage_space_string << lpTotalNumberOfFreeBytes.QuadPart;

            spdlog::info("Drive {} has: {} bytes free for acquisition", m_config->path.string(), lpTotalNumberOfFreeBytes.QuadPart);
            ui.startAcquisitionBtn->setStyleSheet("");
            return true;
       }

        //not enough space for acquisition
        spdlog::error("Not enough space for acquisition");
        ui.startAcquisitionBtn->setToolTip(QString::fromStdString(fmt::format("Not enough space in drive {}", m_config->path.string())));
        ui.startAcquisitionBtn->setStyleSheet("background-color: gray");
        return false;
    } else {
        ui.startAcquisitionBtn->setToolTip("Camera not found.");
        return false;
    }
#else
    return true;
#endif
}

/*
 * @brief Iterate through files in directory and return vector of names
 */
std::vector<std::filesystem::path> MainWindow::getFileNamesFromDirectory(std::filesystem::path path) {
    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::directory_iterator{path}) {
        if (entry.is_regular_file()) {
            files.push_back(entry);
        }
    }
    return files;
}

QStringList MainWindow::vectorToQStringList(const std::vector<std::filesystem::path>& paths) {
    QStringList qStringList;
    for (std::filesystem::path filePath : paths) {
        qStringList.append(QString::fromStdString(filePath.replace_extension().filename().string()));
    }
    return qStringList;
}


/*
 * Updates live view with latest image, this is called
 * by a timer at ~24 FPS.
 */
void MainWindow::updateLiveView() noexcept {
    std::unique_lock<std::mutex> lock(m_liveViewLock);
    if (m_acquisition) {
        pm::Frame* frame = m_acquisition->GetLatestFrame();

        if (frame != nullptr) {
            uint16_t* data = static_cast<uint16_t*>(frame->GetData());

            //Calculate histogram
            m_taskFrameStats->Setup(data, m_hist, m_width, m_height);
            m_parTask.Start(m_taskFrameStats);
            m_taskFrameStats->Results(m_min, m_max, m_hmax);

            float scale = 1.0f;
            float autoMin = 0.0f;

            if (!m_config->noAutoConBright) {
                autoMin = static_cast<float>(m_min / 65535.0f);
                scale = (m_min == m_max) ? 1.0 : 1.0 / ((m_max - m_min) / 65535.0);
            }

            m_liveView->UpdateImage(data, scale, autoMin);
            ui.histView->Update(m_hmax, m_min, m_max);
        }
    }
}


/*
 * @brief PostProcess acquisition data
 */
void MainWindow::postProcess() {
    if (!m_userCanceled) {
        std::vector<toml::value> stagePos;
        std::vector<bool> tileEnabled;

        for (auto& loc : m_stageControl->GetPositions()) {
            stagePos.push_back(toml::value{{"x", loc->x}, {"y", loc->y}});
            tileEnabled.push_back(!loc->skipped);
        }

        std::string rawFile = fmt::format("{}_{}.raw", m_config->prefix, std::string(m_startAcquisitionTS));
        std::string rawFileDownsampled = fmt::format("{}_{}_bin{}.raw", m_config->prefix, std::string(m_startAcquisitionTS), m_config->binFactor);

        writeSettingsFile(m_expSettings.acquisitionDir);

        uint16_t rowsxcols = m_config->rows * m_config->cols;

        if (m_config->autoTile && (rowsxcols != stagePos.size() || rowsxcols != m_config->tileMap.size())) {
            spdlog::warn("Auto tile enabled but acquisition count {} does not match rows * cols {}, skipping", stagePos.size(), rowsxcols);
            return;
            //TODO fix this to use enum values
        } else if (m_expSettings.storageType != 0 && m_expSettings.storageType != 2) { //single tiff file storage, raw file
            spdlog::warn("Auto tile enabled but storage type ({}) is not single image tiff files, skipping", m_expSettings.storageType);
            return;
        } else if (m_config->autoTile) {
            spdlog::info("Autotile: {}, rows: {}, cols: {}, frames: {}, positions: {}", m_config->autoTile, m_config->rows, m_config->cols, m_expSettings.frameCount, stagePos.size());

            std::shared_ptr<RawFile<6>> raw = std::make_shared<RawFile<6>>(
                    (m_expSettings.acquisitionDir / rawFile),
                    16,
                    m_config->cols * m_width,
                    m_config->rows * m_height
                );

            std::shared_ptr<RawFile<6>> rawDownsampled = nullptr;

            if (m_config->enableDownsampleRawFiles) {
                rawDownsampled = std::make_shared<RawFile<6>>(
                    (m_expSettings.acquisitionDir / rawFileDownsampled),
                    16,
                    m_config->cols * (m_width / m_config->binFactor),
                    m_config->rows * (m_height / m_config->binFactor)
                );
            }

            emit sig_progress_start("Tiling images", m_expSettings.frameCount);

            PostProcess::AutoTile(
                (m_expSettings.acquisitionDir / DATA_DIR),
                m_config->prefix,
                m_expSettings.frameCount,
                m_config->rows,
                m_config->cols,
                m_config->tileMap,
                tileEnabled,
                m_width,
                m_height,
                m_config->vflip,
                m_config->hflip,
                !m_config->noAutoConBright,
                [&](size_t n) { emit sig_progress_update(n); },
                raw,
                rawDownsampled,
                m_expSettings.storageType,
                m_config->binFactor
            );

            raw->Close();

            if (m_config->enableDownsampleRawFiles) {
                rawDownsampled->Close();
            }

            emit sig_progress_done();
        }

    }
}


// handle acquisition done signal from thread finished slot
void MainWindow::acquisitionThread(MainWindow* cls) {
    auto progressCB = [&](size_t n) { emit cls->sig_progress_update(n); };
    auto processFrame = [](FrameCtx* frameCtx, pm::Frame* frame) { processing::writeRawFrame(frameCtx, frame); };

    double voltage = (cls->m_config->ledIntensity / 100.0) * cls->m_config->maxVoltage;
    cls->ledON(voltage);

    cls->m_needsPostProcessing = true;

    spdlog::info("Starting acquisitions");
    int pos = 1;

    // get local timestamp to add to subdir name
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    std::tm *tm = std::localtime(&timestamp);

    // make subdirectory to write to
    std::strftime(std::data(cls->m_startAcquisitionTS), std::size(cls->m_startAcquisitionTS), TIMESTAMP_STR, tm);
    std::strftime(std::data(cls->m_recordingDateFmt), std::size(cls->m_recordingDateFmt), RECORDING_DATE_FMT, tm);
    std::string subdir = cls->m_config->prefix + std::string(cls->m_startAcquisitionTS);

    cls->m_expSettings.acquisitionDir = cls->m_expSettings.workingDir / subdir;
    if (!std::filesystem::exists(cls->m_expSettings.acquisitionDir)) {
        spdlog::info("Acquisition being written under directory: {}", cls->m_expSettings.acquisitionDir.string());
        std::filesystem::create_directories(cls->m_expSettings.acquisitionDir);
        std::filesystem::create_directories(cls->m_expSettings.acquisitionDir / DATA_DIR);
    }

    cls->m_expSettings.expTimeMS = (1 / cls->m_config->fps) * 1000;
    cls->m_expSettings.frameCount = cls->m_config->duration * cls->m_config->fps;

    int numActiveFovs = 0;
    for (auto& loc : cls->m_stageControl->GetPositions()) {
        if (!loc->skipped) {
            numActiveFovs++;
        }
    }
    emit cls->sig_progress_start("Acquiring images", numActiveFovs * cls->m_expSettings.frameCount);

    for (auto& loc : cls->m_stageControl->GetPositions()) {
        if (loc->skipped) {
            pos++;
            continue;
        }
        emit cls->sig_disable_ui_moving_stage();
        emit cls->sig_set_platemap(pos);

        spdlog::info("Moving stage, x: {}, y: {}", loc->x, loc->y);
        emit cls->sig_progress_text("Moving stage");
        cls->m_stageControl->SetAbsolutePosition(loc->x, loc->y);
        emit cls->sig_enable_ui_moving_stage();

        cls->m_expSettings.filePrefix = fmt::format("{}_{}_", cls->m_config->prefix, pos++);

        if (!cls->m_acquisition) {
            cls->m_acquisition = std::make_unique<pmAcquisition>(cls->m_camera);
        }

        cls->m_acquisition->StopAll();
        cls->m_acquisition->WaitForStop();

        cls->m_expSettings.trigMode = cls->m_config->triggerMode;
        cls->m_camera->UpdateExp(cls->m_expSettings);

        emit cls->sig_progress_text(fmt::format("Acquiring images for position ({}, {})", loc->x, loc->y));
        cls->m_acquisition->StartAcquisition(progressCB, processFrame);

        if (cls->m_curState == LiveViewAcquisitionRunning || cls->m_curState == LiveViewRunning) {
            cls->m_acquisition->StartLiveView();
        }

        spdlog::info("Waiting for acquisition");
        cls->m_acquisition->WaitForAcquisition();

        //TODO check for user cancel and jump out
        if (cls->m_userCanceled) {
            spdlog::info("User canceled acquisition");
            cls->m_needsPostProcessing = false;
            break;
        }

        spdlog::info("Acquisition for location x: {}, y: {} finished", loc->x, loc->y);
    }
    emit cls->sig_set_platemap(0);

    uint16_t rowsxcols = cls->m_config->rows * cls->m_config->cols;
    bool sizeMatches = (rowsxcols == cls->m_stageControl->GetPositions().size() && rowsxcols == cls->m_config->tileMap.size());

    if (cls->m_config->autoTile && sizeMatches && cls->m_needsPostProcessing) {
        emit cls->sig_update_state(PostProcessing);
    } else if(!cls->m_userCanceled) {
        spdlog::info("Acquisition done, sending signal");
        emit cls->sig_update_state(AcquisitionDone);
    }

    spdlog::info("Acquisition Thread Stopped");
}


// handle acquisition done signal from thread finished slot
void MainWindow::backgroundRecordingThread(MainWindow* cls) {
    if (cls->m_config->plateFormat == "") {
        spdlog::error("Platemap format is not set");
        return;
    }

    //progress bar callback
    auto progressCB = [&](size_t n) { emit cls->sig_progress_update(n); };
    std::vector<std::tuple<uint32_t, uint32_t>> rois = Rois::roiOffsets(&cls->m_roiCfg, cls->m_width, cls->m_height);

    //TODO parameterize intensity count
    std::vector<double> *wellAvgs[3];
    for (auto &v : wellAvgs) {
        v = new std::vector<double>[rois.size() * cls->m_config->rows * cls->m_config->cols]{};
    }

    //Fall back to unvectorized version if we don't know about the roi size for a platemap
    //otherwise use the specialized version
    auto roiFn = processing::roiAvgGeneric;
    if (cls->m_roiCfg.width / cls->m_roiCfg.scale == 16 && cls->m_roiCfg.height / cls->m_roiCfg.scale == 16) { roiFn = processing::roiAvg<16, 16>; }
    else if (cls->m_roiCfg.width / cls->m_roiCfg.scale == 32 && cls->m_roiCfg.height / cls->m_roiCfg.scale == 32) { roiFn = processing::roiAvg<32, 32>; }
    else if (cls->m_roiCfg.width / cls->m_roiCfg.scale == 40 && cls->m_roiCfg.height / cls->m_roiCfg.scale == 40) { roiFn = processing::roiAvg<40, 40>; }
    else if (cls->m_roiCfg.width / cls->m_roiCfg.scale == 64 && cls->m_roiCfg.height / cls->m_roiCfg.scale == 64) { roiFn = processing::roiAvg<64, 64>; }
    else if (cls->m_roiCfg.width / cls->m_roiCfg.scale == 32 && cls->m_roiCfg.height / cls->m_roiCfg.scale == 16) { roiFn = processing::roiAvg<32, 16>; }
    else if (cls->m_roiCfg.width / cls->m_roiCfg.scale == 64 && cls->m_roiCfg.height / cls->m_roiCfg.scale == 32) { roiFn = processing::roiAvg<64, 32>; }
    else if (cls->m_roiCfg.width / cls->m_roiCfg.scale == 110 && cls->m_roiCfg.height / cls->m_roiCfg.scale == 64) { roiFn = processing::roiAvg<110, 64>; }
    else if (cls->m_roiCfg.width / cls->m_roiCfg.scale == 128 && cls->m_roiCfg.height / cls->m_roiCfg.scale == 64) { roiFn = processing::roiAvg<128, 64>; }
    else if (cls->m_roiCfg.width / cls->m_roiCfg.scale == 128 && cls->m_roiCfg.height / cls->m_roiCfg.scale == 128) { roiFn = processing::roiAvg<128, 128>; }
    else if (cls->m_roiCfg.width / cls->m_roiCfg.scale == 220 && cls->m_roiCfg.height / cls->m_roiCfg.scale == 128) { roiFn = processing::roiAvg<220, 128>; }
    else if (cls->m_roiCfg.width / cls->m_roiCfg.scale == 256 && cls->m_roiCfg.height / cls->m_roiCfg.scale == 128) { roiFn = processing::roiAvg<256, 128>; }
    else if (cls->m_roiCfg.width / cls->m_roiCfg.scale == 256 && cls->m_roiCfg.height / cls->m_roiCfg.scale == 256) { roiFn = processing::roiAvg<256, 256>; }

    //process frame callback
    auto processFrame = [&](size_t intensityIdx, size_t fovIdx) {
        auto plateCols = cls->m_config->cols;
        auto plateRows = cls->m_config->rows;
        auto wellsPerRow = cls->m_roiCfg.cols * plateCols;

        return [&, wellsPerRow, intensityIdx, fovIdx, plateCols, plateRows](FrameCtx* frameCtx, pm::Frame* frame) {
            for (auto r = 0; r < cls->m_roiCfg.rows; r++) {
                for (auto c = 0; c < cls->m_roiCfg.cols; c++) {
                    auto idx = fovIdx * cls->m_roiCfg.rows * cls->m_roiCfg.cols;
                    idx += c + r*cls->m_roiCfg.cols;

                    size_t x, y;
                    std::tie(x, y) = rois[c + r*cls->m_roiCfg.cols];

                    double avg = roiFn(&cls->m_roiCfg, (uint16_t*)(frame->GetData()), x, y, frameCtx->width);
                    wellAvgs[intensityIdx][idx].push_back(avg);
                }
            }
        };
    };

    std::vector<double> ledIntensities = {
        cls->m_config->ledIntensity,
        0.5 * cls->m_config->ledIntensity,
        0.25 * cls->m_config->ledIntensity,
    };

    cls->ledON((cls->m_config->ledIntensity / 100.0) * cls->m_config->maxVoltage);
    cls->m_needsPostProcessing = false;

    spdlog::info("Starting background recording thread");
    cls->m_expSettings.expTimeMS = (1 / cls->m_config->fps) * 1000;

    // only need 1 sec of data for background recordings
    cls->m_expSettings.frameCount = cls->m_config->fps;

    auto stagePositions = cls->m_stageControl->GetPositions();
    emit cls->sig_progress_start("Acquiring images", stagePositions.size() * ledIntensities.size() * cls->m_expSettings.frameCount);

    for (auto [fovIdx, loc] : stagePositions | std::views::enumerate) {
        emit cls->sig_disable_ui_moving_stage();
        emit cls->sig_set_platemap(fovIdx+1);

        spdlog::info("Moving stage, x: {}, y: {}", loc->x, loc->y);
        emit cls->sig_progress_text("Moving stage");
        cls->m_stageControl->SetAbsolutePosition(loc->x, loc->y);
        emit cls->sig_enable_ui_moving_stage();

        if (!cls->m_acquisition) {
            cls->m_acquisition = std::make_unique<pmAcquisition>(cls->m_camera);
        }

        cls->m_acquisition->StopAll();
        cls->m_acquisition->WaitForStop();

        cls->m_expSettings.trigMode = cls->m_config->triggerMode;
        cls->m_camera->UpdateExp(cls->m_expSettings);

        emit cls->sig_progress_text(fmt::format("Acquiring images for position ({}, {})", loc->x, loc->y));

        for (auto [i, intensity] : ledIntensities | std::views::enumerate) {
            auto frameFn = processFrame(i, cls->m_config->tileMap[fovIdx]);

            cls->ledSetVoltage((intensity / 100.0) * cls->m_config->maxVoltage);
            cls->m_acquisition->StartAcquisition(progressCB, frameFn);

            if (cls->m_curState == LiveViewAcquisitionRunning || cls->m_curState == LiveViewRunning) {
                cls->m_acquisition->StartLiveView();
            }

            cls->m_acquisition->WaitForAcquisition();
        }
        spdlog::info("Background Recording for location x: {}, y: {} finished", loc->x, loc->y);
    }

    std::vector<double> wellAverageIntensity[3];

    for (auto i = 0; i < ledIntensities.size(); i++) {
        for (auto w = 0; w < rois.size() * cls->m_config->rows * cls->m_config->cols; w++) {

            //drop first and last 0.1 seconds of data and average each well per led intensity
            auto n = 0.1 * wellAvgs[i][w].size();
            wellAvgs[i][w].erase(wellAvgs[i][w].begin(), wellAvgs[i][w].begin() + n);
            wellAvgs[i][w].resize(wellAvgs[i][w].size() - n);

            //avgerage intensity for well
            double wavg = std::reduce(wellAvgs[i][w].begin(), wellAvgs[i][w].end(), 0.0) / wellAvgs[i][w].size();
            wellAverageIntensity[i].push_back(wavg);
        }
    }

    if (cls->m_config->plateId != "") {
        std::ofstream backgroundFile;

        // get local timestamp to add to subdir name
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);
        std::tm *tm = std::localtime(&timestamp);

        // make subdirectory to write to
        std::strftime(std::data(cls->m_startAcquisitionTS), std::size(cls->m_startAcquisitionTS), TIMESTAMP_STR, tm);
        std::strftime(std::data(cls->m_recordingDateFmt), std::size(cls->m_recordingDateFmt), RECORDING_DATE_FMT, tm);
        std::string bgname = cls->m_config->plateId + ".tsv";

        std::filesystem::path dir = cls->m_config->backgroundRecordingDir / cls->m_config->plateId / bgname;
        spdlog::info("Writing background recording to {}", dir.string());

        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directories(cls->m_config->backgroundRecordingDir);
            std::filesystem::create_directories(cls->m_config->backgroundRecordingDir / cls->m_config->plateId);
        }

        backgroundFile.open(dir.string());
        backgroundFile << "Well\t" << "Background Fluorescence, 100% LED Intensity (AU)\t"
                       << "Background Fluorescence, 50% LED Intensity (AU)\t"
                       << "Background Fluorescence, 25% LED Intensity (AU)"
                       << std::endl;


        std::vector<std::tuple<size_t, std::string>> file_rows{};

        for (auto fovIdx = 0; fovIdx < cls->m_config->cols * cls->m_config->rows; fovIdx++) {
            for (auto r = 0; r < cls->m_roiCfg.rows; r++) {
                for (auto c = 0; c < cls->m_roiCfg.cols; c++) {
                    //handle image flips
                    auto c_adj = (cls->m_config->hflip) ? cls->m_roiCfg.cols - c - 1 : c;
                    auto r_adj = (cls->m_config->vflip) ? cls->m_roiCfg.rows - r - 1 : r;
                    auto idx = (c_adj + r_adj * cls->m_roiCfg.cols) + (fovIdx * cls->m_roiCfg.cols * cls->m_roiCfg.rows);

                    //don't need to flip these because the idx is already flipped
                    auto col = c + (fovIdx % cls->m_config->cols) * cls->m_roiCfg.cols;
                    auto row = r + (cls->m_roiCfg.rows * (fovIdx / cls->m_config->cols));
                    auto wellIdx = row * (cls->m_roiCfg.rows * cls->m_config->cols) + col;

                    std::vector<double> file_row;
                    for (auto i = 0; i < ledIntensities.size(); i++) {
                        file_row.push_back(wellAverageIntensity[i][idx]);
                    }

                    std::string well_name = Rois::wellName(row, col);
                    file_rows.push_back(std::make_tuple(wellIdx, fmt::format("{}\t{}", well_name, fmt::join(file_row, "\t"))));
                }
            }
        }

        std::sort(file_rows.begin(), file_rows.end());
        for (auto fr : file_rows) {
            backgroundFile << std::get<1>(fr) << std::endl;
        }

        backgroundFile.close();
        cls->writeSettingsFile(cls->m_config->backgroundRecordingDir / cls->m_config->plateId);
    }

    for (auto& v : wellAvgs) { delete[] v; }
    cls->saveBackgroundRecordingMetadata();

    //write settings file
    spdlog::info("Writing settings file to {}\\{}\\settings.toml", cls->m_config->backgroundRecordingDir.string(), cls->m_config->plateId);

    emit cls->sig_set_platemap(0);
    emit cls->sig_update_state(AcquisitionDone);

    spdlog::info("Background Recording Thread Stopped");
}


void MainWindow::sendManualTrigger() {
    spdlog::info("User is sending manual trigger");
    uint8_t on_lines[8] = {1,1,1,1,1,1,1,1};
    uint8_t off_lines[8] = {0,0,0,0,0,0,0,0};

    bool taskDO_2_result = (
        m_DAQmx.StartTask(m_trigTaskDO) && \
        m_DAQmx.WriteDigitalLines(m_trigTaskDO, 1, 0, 10.0, DAQmx_Val_GroupByChannel, off_lines, NULL) && \
        m_DAQmx.WriteDigitalLines(m_trigTaskDO, 1, 0, 10.0, DAQmx_Val_GroupByChannel, on_lines, NULL) && \
        m_DAQmx.StopTask(m_trigTaskDO)
    );

    if (!taskDO_2_result) {
        spdlog::error("Failed to send manual trigger");
        m_DAQmx.StopTask(m_trigTaskDO);
    }
}

void MainWindow::writeSettingsFile(std::filesystem::path fp) {
    spdlog::info("Writing settings file to {}\\settings.toml", fp.string());
    std::ofstream outfile((fp / "settings.toml").string()); // create output file stream

    //need this here even if auto tile is disabled
    std::string rawFile = fmt::format("{}_{}.raw", m_config->prefix, std::string(m_startAcquisitionTS));
    std::string rawFileDownsampled = fmt::format("{}_{}_bin{}.raw", m_config->prefix, std::string(m_startAcquisitionTS), m_config->binFactor);


    //output capture settings
    const toml::basic_value<toml::preserve_comments, tsl::ordered_map> settings{
        { "instrument_name", "Nautilai" },
        { "software_version", m_config->version },
        { "recording_date", m_recordingDateFmt },
        { "led_intensity", m_config->ledIntensity },
        { "auto_contrast_brightness", !m_config->noAutoConBright },
        { "fps", m_config->fps },
        { "duration", m_config->duration },
        { "num_frames", m_expSettings.frameCount },
        { "scale_factor", m_config->rgn.sbin }, //TODO not sure if this is right?
        { "bit_depth", m_camInfo.spdTable[m_config->spdtable].bitDepth },
        { "vflip", m_config->vflip },
        { "hflip", m_config->hflip },
        { "auto_tile", m_config->autoTile },
        { "width", m_width },
        { "height", m_height },
        { "num_horizontal_pixels", m_config->cols * m_width },
        { "num_vertical_pixels", m_config->rows * m_height },
        { "rows", m_config->rows },
        { "cols", m_config->cols },
        { "xy_pixel_size", m_config->xyPixelSize },
        { "data_type", ui.dataTypeList->currentText().toStdString() },
        { "plate_id", m_config->plateId },
        { "use_background_subtraction", m_config->useBackgroundSubtraction }
    };
    outfile << std::setw(100) << settings << std::endl;

    if (m_config->enableDownsampleRawFiles) {
        const toml::basic_value<toml::preserve_comments, tsl::ordered_map> binSettings{
            { "additional_bin_factor", m_config->binFactor },
            { "keep_original", m_config->keepOriginalRaw },
            { "downsampled_input_path", (m_expSettings.acquisitionDir / rawFileDownsampled).string() },
        };

        outfile << std::setw(100) << binSettings << std::endl;
    }

    const toml::basic_value<toml::preserve_comments, tsl::ordered_map> paths {
        { "output_dir_path", m_expSettings.acquisitionDir.string() },
        { "input_path", (m_expSettings.acquisitionDir / rawFile).string() },
        { "background_recording_dir", m_config->backgroundRecordingDir.string() }
    };

    outfile << std::setw(300) << paths << std::endl;

    //output platemap format
    if (m_config->plateFormat != "") {
        auto platemapFormat = toml::parse(m_config->plateFormat);
        outfile << std::setw(100) << platemapFormat << std::endl;
    }

    outfile.close();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    spdlog::info("Close event received");
    if (m_curState == PostProcessing || m_curState == PostProcessingLiveView) {
        spdlog::info("Ignoring close event as post processing is running");
        event->ignore();
    } else {
        m_userCanceled = true;

        if (m_curState == LiveViewRunning) {
            stopLiveView();
        }
        if (m_curState == AcquisitionRunning) {
            stopAcquisition();
        }
        if (m_curState == LiveViewAcquisitionRunning) {
            stopAcquisition_LiveViewRunning();
            stopLiveView();
        }

        if (m_config->updateAvailable) {
            spdlog::info("Ignoring close event, prompting user to confirm/ignore update");
            event->ignore();
            m_autoUpdate->show();
        } else if (m_db) {
            delete m_db;
            m_db = nullptr;
        }
    }
}
