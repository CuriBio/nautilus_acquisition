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

#include <chrono>
#include <ctime>
#include <format>
#include <fstream>
#include <future>
#include <iostream>
#include <stdlib.h>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <fileapi.h>
#endif

#include <spdlog/spdlog.h>
#include <QMessageBox>
#include <QThread>
#include <QTimer>
#include <QProcess>

#include "mainwindow.h"
#include <PostProcess.h>
#include <VideoEncoder.h>
#include <RawFile.h>


/*
 * Instance of the main Nautilus application window.
 *
 * @param path The output path for captured images.
 * @param prefix The prefix value for captured image names.
 * @param niDev The name of the NIDAQmx device controlling the LED light.
 * @param testImgPath The path to a test image used for testing acquisition software.
 * @param fps The initial frames per second setting.
 * @param duration The initial capture duration setting.
 * @param expTimeMs The initial exposure time.
 * @param spdtable The initial speed table index.
 * @param ledIntensity The initial led intensity settings.
 * @param bufferCount The size of circular buffer to use for acquisition.
 * @param frameCount The initial frame count for acquisition.
 * @param storageType The initial image storage type.
 * @param triggerMode The camera trigger mode.
 * @param exposureMode The camera exposure mode.
 * @param maxVoltage The max voltage for the LED controller.
 * @param autoConBright Flag to disable auto contrast/brightness for live view.
 * @param parent Pointer to parent widget.
 */
MainWindow::MainWindow(std::shared_ptr<Config> params, QMainWindow *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    m_config = params;

    m_settings = new Settings(this, m_config->path, m_config->prefix);
    m_stageControl = new StageControl(m_config->stageComPort, m_config, m_config->stageStepSizes, this);


    //Setup NIDAQmx controller for LED
    m_advancedSettingsDialog = new AdvancedSetupDialog(m_config, this);

    //Get all plate format file names
    m_plateFormats = getFileNamesFromDirectory("./plate_formats");

    //setup width/height and initial exposure settings
    m_width = (m_config->rgn.s2 - m_config->rgn.s1 + 1) / m_config->rgn.sbin;
    m_height = (m_config->rgn.p2 - m_config->rgn.p1 + 1) / m_config->rgn.pbin;

    m_expSettings.workingDir = m_config->path;
    m_expSettings.acquisitionDir = m_config->path;
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

    //acquisition progress bar
    m_acquisitionProgress = new QProgressDialog("", "Cancel", 0, 100, this, Qt::WindowStaysOnTopHint);
    m_acquisitionProgress->cancel();

    //show error popup
    connect(this, &MainWindow::sig_show_error, this, [&](std::string msg) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error", msg.c_str());
        messageBox.setFixedSize(500,200);
        if (!m_config->ignoreErrors) { exit(1); }
    });

    //progress bar
    connect(this, &MainWindow::sig_progress_start, this, [&](std::string msg, int n) {
        m_acquisitionProgress->setMinimum(0);
        m_acquisitionProgress->setMaximum(n);
        m_acquisitionProgress->setValue(0);
        m_acquisitionProgress->setLabelText(QString::fromStdString(msg));
        m_acquisitionProgress->show();
    });
    connect(this, &MainWindow::sig_progress_text, this, [&](std::string msg) {
        m_acquisitionProgress->setLabelText(QString::fromStdString(msg));
    });
    connect(this, &MainWindow::sig_progress_update, this, [&](size_t n) {
        m_acquisitionProgress->setValue(m_acquisitionProgress->value() + n);
    });
    connect(this, &MainWindow::sig_progress_done, this, [&]() {
        m_acquisitionProgress->setValue(m_acquisitionProgress->maximum());
    });
    connect(m_acquisitionProgress, &QProgressDialog::canceled, this, [&] {
        m_userCanceled = true;
        on_startAcquisitionBtn_clicked();
        //StopAcquisition();
    });

    connect(this, &MainWindow::sig_acquisition_done, this, &MainWindow::acquisition_done);
    connect(m_settings, &Settings::sig_settings_changed, this, &MainWindow::settings_changed);
    connect(m_advancedSettingsDialog, &AdvancedSetupDialog::sig_ni_dev_change, this, &MainWindow::setupNIDev);

    //fps, duration update
    connect(this, &MainWindow::sig_set_fps_duration, this, [&](int maxfps, int fps, int duration) {
        ui.frameRateEdit->setMaximum(maxfps);
        ui.frameRateEdit->setValue((m_config->fps <= maxfps) ? m_config->fps : maxfps);
        ui.durationEdit->setValue(m_config->duration);
    });

    //set platmapFormat
    connect(this, &MainWindow::sig_set_platmapFormat, this, [&](QStringList qs) {
        ui.plateFormatDropDown->addItems(qs);
        ui.plateFormatDropDown->setCurrentIndex(-1);
    });

    //stage control signals
    connect(m_stageControl, &StageControl::sig_stagelist_updated, this, &MainWindow::stagelist_updated);

    /*
     * Start external analysis
     */
    connect(&m_extAnalysis, &QProcess::started, this, [this] {
        spdlog::info("Process started");
    });

    connect(&m_extAnalysis, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        spdlog::info("External process finished, exitCode {}, exitStatus {}", exitCode, exitStatus);
        m_extEncodingRetries = 0;
        ui.startAcquisitionBtn->setText("Start Acquisition");

        //need to check if there is enough space for another acquisition
        availableDriveSpace(m_config->fps, m_config->duration, m_stageControl->GetPositions().size());
    });

    connect(&m_extAnalysis, &QProcess::errorOccurred, this, [&](QProcess::ProcessError err) {
            spdlog::error("External analysis error: {}", err);
            ui.startAcquisitionBtn->setText("Start Acquisition");

            //need to check if there is enough space for another acquisition
            availableDriveSpace(m_config->fps, m_config->duration, m_stageControl->GetPositions().size());
    });

    connect(this, &MainWindow::sig_start_analysis, this, [&] {
        //run external analysis, probably want to start another progress bar/spinner
        std::filesystem::path settingsPath = m_expSettings.acquisitionDir / "settings.toml";
        spdlog::info("Starting external analysis {} with {}", m_config->extAnalysis.string(), settingsPath.string());

        m_extAnalysis.setProcessChannelMode(QProcess::ForwardedChannels);
        m_extAnalysis.start(QString::fromStdString(m_config->extAnalysis.string()), QStringList() << settingsPath.string().c_str());
    });

    /*
     *  Start video encoding
     */
    connect(this, &MainWindow::sig_start_encoding, this, [&] {
        //run external video encoder command
        std::string encodingCmd = fmt::format("\"{}\" -f rawvideo -pix_fmt gray12le -r {} -s:v {}:{} -i {} {}",
                        m_config->ffmpegDir.string(),
                        std::to_string(m_config->fps),
                        std::to_string(m_width * m_config->cols),
                        std::to_string(m_height * m_config->rows),
                        fmt::format("\"{}_{}.raw\"", (m_expSettings.acquisitionDir / m_config->prefix).string(), std::string(m_startAcquisitionTS)),
                        fmt::format("\"{}_stack_{}.avi\"", (m_expSettings.acquisitionDir / m_config->prefix).string(), std::string(m_startAcquisitionTS))
                      );

        spdlog::info("Starting video encoding {}", encodingCmd);

        m_extVidEncoder.setProcessChannelMode(QProcess::ForwardedChannels);
        m_extVidEncoder.start(QString::fromStdString(encodingCmd));
    });

    connect(&m_extVidEncoder, &QProcess::started, this, [this] {
        spdlog::info("Process started");
    });

    connect(&m_extVidEncoder, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        spdlog::info("Video encoding finished");
    });

    connect(&m_extVidEncoder, &QProcess::errorOccurred, this, [&](QProcess::ProcessError err) {
        if (++m_extEncodingRetries < 5) {
            double backoff = m_extRetryBackoffms * std::pow(m_extEncodingRetries, 2);
            spdlog::error("External video encoding error: {}, retrying {} with backoff {}ms", err, m_extEncodingRetries, backoff);

            std::thread t([&] {
                std::this_thread::sleep_for(std::chrono::duration<double>(backoff / 1000.0)); //in seconds
                emit sig_start_encoding();
            });
            t.detach();
        } else {
            spdlog::error("External video encoding failed: {}", err);
        }
    });

    //live view timer signals
    m_liveViewTimer = new QTimer(this);
    connect(m_liveViewTimer, &QTimer::timeout, this, &MainWindow::updateLiveView);

    //initialization signals
    connect(this, &MainWindow::sig_enable_controls, this, &MainWindow::EnableAll);

    //initialize histogram buffer
    m_hist = new uint32_t[(1<<16) - 1];
    memset((void*)m_hist, 0, sizeof(uint32_t)*((1<<16)-1));

    //initialize lut buffer
    m_lut16 = new uint8_t[(1<<16) - 1];
    memset((void*)m_lut16, 0, (1<<16)-1);

    //create task pools
    m_taskFrameStats = std::make_shared<TaskFrameStats>(TASKS);
    m_taskUpdateLut = std::make_shared<TaskFrameLut16>();
    m_taskApplyLut = std::make_shared<TaskApplyLut16>();
}


/*
 * Initializes main window and camera/acquisition objects.
 */
void MainWindow::Initialize() {
    //disable all controls while initializing

    emit sig_enable_controls(false);
    //
    //check if stage connected and show error
    if (!m_stageControl->Connected()) {
        emit sig_show_error("Stage not found, please plug in all devices and restart applicatoin");
    }

    //Async calibrate stage
    if (m_config->asyncInit) {
        m_stageCalibrate = std::async(std::launch::async, [&] {
            return m_stageControl->Calibrate();
        });

        m_niSetup = std::async(std::launch::async, [&] {
            setupNIDev(m_config->niDev);
            m_advancedSettingsDialog->Initialize(m_DAQmx.GetListOfDevices());
        });
    } else {
        //calibrate stage
        m_stageControl->Calibrate();

        //setup NI device
        setupNIDev(m_config->niDev);
        m_advancedSettingsDialog->Initialize(m_DAQmx.GetListOfDevices());
    }

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

    //for 8 bit image conversion for liveview, might not need it anymore
    m_img16 = new uint16_t[m_width*m_height];

    //Set sensor size for live view
    ui.liveView->Init(m_width, m_height, m_config->vflip, m_config->hflip, ImageFormat::Mono16);
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
    double max_fps = calcMaxFrameRate(m_expSettings.region.p1, m_expSettings.region.p2, m_config->lineTimes[m_expSettings.spdTableIdx]);
    spdlog::info("Max frame rate: {}", max_fps);

    emit sig_set_fps_duration(max_fps, (m_config->fps <= max_fps) ? m_config->fps : max_fps, m_config->duration);

    //set options for plate formats drop down
    emit sig_set_platmapFormat(vectorToQStringList(m_plateFormats));

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

    //enable ui
    emit sig_enable_controls(true);
}

void MainWindow::EnableAll(bool enable) {
    //set cursor if waiting for ui init
    if (!enable) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    } else {
        QApplication::restoreOverrideCursor();
    }

    //probably should be moved, but when the QComboBox is enabled after being disabled
    //it resets the index to 0, which is not what we want on startup
    ui.plateFormatDropDown->setEnabled(enable);
    if (enable) {
        ui.plateFormatDropDown->setCurrentIndex(-1);
    }

    ui.durationEdit->setEnabled(enable);
    ui.frameRateEdit->setEnabled(enable);
    ui.ledIntensityEdit->setEnabled(enable);
    ui.startAcquisitionBtn->setEnabled(enable);
    ui.liveScanBtn->setEnabled(enable);
    ui.stageNavigationBtn->setEnabled(enable);
    ui.advancedSetupBtn->setEnabled(enable);
    ui.settingsBtn->setEnabled(enable);
}


/*
* Runs when a new ni device is selected, re configure ni device leds
*/
void MainWindow::setupNIDev(std::string niDev) {
    m_config->niDev = niDev;

    //Setup NIDAQmx controller for LED
    m_taskAO = "Analog_Out_Volts"; //Task for setting Analog Output voltage
    m_devAO = fmt::format("{}/ao0", m_config->niDev); //Device name for analog output
    spdlog::info("Using NI device {} for analog output", m_devAO);
    m_DAQmx.ClearTask(m_taskAO);

    m_taskDO = "Digital_Out"; //Task for setting Digital Output
    m_devDO = fmt::format("{}/port0/line0:7", m_config->niDev); //Device for digital output
    spdlog::info("Using NI device {} for digital output", m_devDO);
    m_DAQmx.ClearTask(m_taskDO);

    m_DAQmx.CreateTask(m_taskAO);
    m_DAQmx.CreateTask(m_taskDO);

    m_DAQmx.CreateAnalogOutpuVoltageChan(m_taskAO, m_devAO.c_str(), -10.0, 10.0, DAQmx_Val_Volts);
    m_DAQmx.CreateDigitalOutputChan(m_taskDO, m_devDO.c_str(), DAQmx_Val_ChanForAllLines);
}


/*
 * Led Intensity edit box slot, called when user changes the led intensity value.
 *
 * @param value The updated led intensity value.
 */
void MainWindow::on_ledIntensityEdit_valueChanged(double value) {
    m_config->ledIntensity = value;
    double voltage = (m_config->ledIntensity / 100.0) * m_config->maxVoltage;
    ledSetVoltage(voltage);
}


/*
 * Check if default drive on windows has sufficient space for acquisition.
 * Update startAcquisitionBtn if error.
 *
 * @param fps setting of acquisition
 * @param duration of acquisition
 *
 * @returns boolean true if space is available
*/
bool MainWindow::availableDriveSpace(double fps, double duration, size_t nStagePositions) {
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
            ui.startAcquisitionBtn->setToolTip("");
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
 * Frame Rate edit box slot, called when users changes the FPS value.
 *
 * @param value The updated FPS value.
 */
void MainWindow::on_frameRateEdit_valueChanged(double value) {
    if (value * m_config->duration < 1.0) {
        spdlog::error("Capture is set to less than 1 frame, fps: {}, duration: {}", value, m_config->duration);
        ui.frameRateEdit->setStyleSheet("background-color: red");
        ui.durationEdit->setStyleSheet("background-color: red");
    } else {
        ui.frameRateEdit->setStyleSheet("background-color: white");
        ui.durationEdit->setStyleSheet("background-color: white");

        m_config->fps = value;
        m_expSettings.expTimeMS = (1 / m_config->fps) * 1000;
        m_expSettings.frameCount = m_config->duration * m_config->fps;

        spdlog::info("Setting new exposure value, fps: {}, frame count: {}, exposure time ms: {}", m_config->fps, m_expSettings.frameCount, m_expSettings.expTimeMS);
    }

    if (m_liveScanRunning) {
        m_liveViewTimer->stop();
        double minFps = std::min<double>(m_config->fps, 24.0);
        m_liveViewTimer->start(int32_t(1000 * (1.0 / minFps)));
    }

    availableDriveSpace(m_config->fps, m_config->duration, m_stageControl->GetPositions().size());
}


void MainWindow::on_plateFormatDropDown_activated(int index){
    m_config->plateFormat = m_plateFormats[index];
    m_stageControl->loadList(m_config->plateFormat.string());
}


/*
 * Duration edit box slot, called when users changes duration value.
 *
 * @param value The updated duration value in seconds.
 */
void MainWindow::on_durationEdit_valueChanged(double value) {
    if (value * m_config->fps < 1.0) {
        spdlog::error("Capture is set to less than 1 frame, fps: {}, duration: {}", value, m_config->duration);
        ui.frameRateEdit->setStyleSheet("background-color: red");
        ui.durationEdit->setStyleSheet("background-color: red");
    } else {
        ui.frameRateEdit->setStyleSheet("background-color: white");
        ui.durationEdit->setStyleSheet("background-color: white");
    }

    m_config->duration = value;
    m_expSettings.expTimeMS = (1 / m_config->fps) * 1000;
    m_expSettings.frameCount = m_config->duration * m_config->fps;

    spdlog::info("Setting new exposure value, fps: {}, frame count: {}, exposure time ms: {}", m_config->fps, m_expSettings.frameCount, m_expSettings.expTimeMS);

    availableDriveSpace(m_config->fps, m_config->duration, m_stageControl->GetPositions().size());
}


/*
 * Advanced setup button slot, called when user clicks on advanced setup button.
 */
void MainWindow::on_advancedSetupBtn_clicked() {
    m_advancedSettingsDialog->show();
}


/*
 * Settings button slot, called when user clicks on settings button.
 * Currently displays settings dialog.
 */
void MainWindow::on_settingsBtn_clicked() {
    m_settings->exec();
}


/*
 * Live Scan button slot, called when user clicks on live scan button.
 * Will toggle live scan from stopped to running or running to stopped
 * depending on the current state.
 */
void MainWindow::on_liveScanBtn_clicked() {
    spdlog::info("liveScanBtn clicked. m_liveScanRunning: {}, m_acquisitionRunning: {}", m_liveScanRunning, m_acquisitionRunning);

    if (!m_liveScanRunning) {
        spdlog::info("Starting live scan");
        m_liveScanRunning = true;
        m_userCanceled = false;

        // max frame rate allowed in live scan is 24, acquisition can capture at higher frame rates
        double minFps = std::min<double>(m_config->fps, 24.0);
        m_liveViewTimer->start(int32_t(1000 * (1.0 / minFps)));

        if (!m_acquisitionRunning) {
            StartAcquisition(false);
        }
        ui.liveScanBtn->setText("Stop Live Scan");
    } else {
        m_liveScanRunning = false;

        if (!m_acquisitionRunning) {
            // this will also stop the live view timer
            m_userCanceled = true;
            StopAcquisition();
        } else {
            m_liveViewTimer->stop();
        }
    }

    ui.frameRateEdit->setEnabled(!m_liveScanRunning);
}


/*
 * Start Acquisition button slot, called when user clicks on start acquisition.
 */
void MainWindow::on_startAcquisitionBtn_clicked() {
    if (!m_acquisitionRunning) {
        spdlog::info("on_startAcquisitionBtn_clicked");
        ui.startAcquisitionBtn->setText("Stop Acquisition");
        m_acquisitionRunning = true;
        m_userCanceled = false;
        StartAcquisition(true);
    } else {
        m_userCanceled = true;

        if (m_liveScanRunning) { // keep livescan running
            emit sig_progress_done();
            m_acquisition->Stop();
        } else {
            StopAcquisition();
        }

        m_acquisitionRunning = false;
        ui.startAcquisitionBtn->setText("Start Acquisition");
    }
}

/*
 * Starts a new acquisition only if acquisition is currently stopped.
 * This method will first start the LED, update the exposure settings
 * based on user input (fps, duration, output dir) and then start the
 * acquisition. If `saveToDisk` is false then not images will be captured.
 *
 * @param saveToDisk Flag to tell acquisition if images should be streamed
 * to disk or only to live view.
 */
void MainWindow::StartAcquisition(bool saveToDisk) {
    std::unique_lock<std::mutex> lock(m_lock);

    if (m_config->duration > 0 && m_config->fps > 0) {
        if (!m_acquisition) {
            spdlog::error("m_acquisition is invalid");
            return;
        }

        if (!m_led && m_config->ledIntensity > 0.0) {
            m_led = !m_led;
            double voltage = (m_config->ledIntensity / 100.0) * m_config->maxVoltage;
            spdlog::info("Setting led intensity {}, voltage {}, max voltage {}", m_config->ledIntensity, voltage, m_config->maxVoltage);
            ledON(voltage);
        }

        switch (m_acquisition->GetState()) {
            case AcquisitionState::AcqStopped:
                {
                    spdlog::info("StartAcquisition, current state AcqStopped");

                    double expTimeMs = (1.0 / m_config->fps) * 1000;
                    spdlog::info("Setting expTimeMS: {} ({})", static_cast<uint32_t>(expTimeMs), expTimeMs);

                    m_expSettings.expTimeMS = static_cast<uint32_t>(expTimeMs);
                    m_expSettings.frameCount = uint32_t(m_config->duration * m_config->fps);
                    m_camera->UpdateExp(m_expSettings);
                    spdlog::info("Starting acquisition: expTimeMS {}, frameCount {}", m_expSettings.expTimeMS, m_expSettings.frameCount);

                    if (!m_acqusitionThread) {
                        m_acqusitionThread = QThread::create(MainWindow::acquisitionThread, this);
                        m_acqusitionThread->start();
                    } else {
                        emit sig_progress_start("Acquiring images", m_expSettings.frameCount * m_stageControl->GetPositions().size());
                        m_acquisition->Start(saveToDisk, [&](size_t n) { emit sig_progress_update(n); }, 0.0, nullptr);
                    }
                }
                break;
            case AcquisitionState::AcqLiveScan:
                spdlog::info("StartAcquisition, current state AcqLiveScan");

                if (!m_acqusitionThread) {
                    m_acqusitionThread = QThread::create(MainWindow::acquisitionThread, this);
                    m_acqusitionThread->start();
                } else {
                    //need to stop first before starting capture otherwise liveview will block
                    spdlog::info("Signal live view to stop");
                    m_acquisition->Stop();
                }
                break;
            case AcquisitionState::AcqCapture:
                spdlog::info("StartAcquisition, current state AcqCapture");
                spdlog::warn("Acquisition is already running");
                break;
            default:
                spdlog::error("Error: Invalid state");
                break;
        }

        //disable controls when capture is running, ignore for livescan only
        if (saveToDisk) {
            ui.frameRateEdit->setEnabled(false);
            ui.durationEdit->setEnabled(false);
            ui.ledIntensityEdit->setEnabled(false);
        }
    }
}


/*
 * Stops a running acquisition and turns off LED.
 */
void MainWindow::StopAcquisition() {
    spdlog::info("StopAcquisition called");
    std::unique_lock<std::mutex> lock(m_lock);
    if (!m_acquisition) {
        spdlog::error("m_acquisition is invalid");
        return;
    }

    switch (m_acquisition->GetState()) {
        case AcquisitionState::AcqLiveScan:
            if (m_liveScanRunning) {
                spdlog::info("Stopping live Scan");
                m_liveViewTimer->stop();
                m_liveScanRunning = false;
            }
            if (!m_acquisitionRunning) {
                spdlog::info("Stopping acquisition");
                m_acquisition->Stop();
            } else {
                spdlog::info("Stopping live scan, capture still running");
            }
            break;
        case AcquisitionState::AcqCapture:
            spdlog::info("Stopping Capture");
            if (m_acquisitionRunning && !m_liveScanRunning) {
                //shut it all down
                spdlog::info("Stopping capture");
                m_acquisitionRunning = false;
                m_acquisition->Stop();
            } else if (m_acquisitionRunning && m_liveScanRunning) {
                //only stop capture, keep live scan running
                spdlog::info("Stopping capture, live view still running");
                m_acquisition->Start(false, [&](size_t n) { emit sig_progress_update(n); }, 0.0, nullptr);
                m_acquisitionRunning = false;
            }
            break;
        case AcquisitionState::AcqStopped:
            spdlog::info("Acquisition not running");
            break;
        default:
            spdlog::error("Error: Invalid state");
            break;
    }

    //enable controls when capture is finished
    ui.frameRateEdit->setEnabled(true);
    ui.durationEdit->setEnabled(true);
    ui.ledIntensityEdit->setEnabled(true);
}


/*
 * Updates live view with latest image, this is called
 * by a timer at ~24 FPS.
 */
void MainWindow::updateLiveView() {
    if (m_acquisition) {
        pm::Frame* frame = m_acquisition->GetLatestFrame();

        if (frame != nullptr) {
            uint16_t* data = static_cast<uint16_t*>(frame->GetData());

            //Calculate histogram
            m_taskFrameStats->Setup(data, m_hist, m_width, m_height);
            m_parTask.Start(m_taskFrameStats);
            m_taskFrameStats->Results(m_min, m_max, m_hmax);

            if (!m_config->noAutoConBright) {
                AutoConBright(data);
                ui.liveView->UpdateImage((uint16_t*)m_img16);
            } else {
                ui.liveView->UpdateImage(data);
            }

            ui.histView->Update(m_hmax, m_min, m_max);
        }
    }
}


/*
 * Applies the auto contrast and brightness algorithm
 * to the current live view image if enabled.
 *
 * @param data Raw pixel data to run auto contrast/brightness on.
 */
void MainWindow::AutoConBright(const uint16_t* data) {
    /* //Update lut */
    m_taskUpdateLut->Setup(m_min, m_max);
    m_parTask.Start(m_taskUpdateLut);
    uint16_t* lut = m_taskUpdateLut->Results();

    //Apply lut
    m_taskApplyLut->Setup(data, m_img16, lut, m_width * m_height);
    m_parTask.Start(m_taskApplyLut);
}


/*
 * Signal to indicate acquisition has finished.
 */
void MainWindow::acquisition_done(bool runPostProcess) {
    spdlog::info("Acquisition done signal");
    std::unique_lock<std::mutex> lock(m_lock);

    if (!m_liveScanRunning) { //livescan isn't running
        if (m_led) {
            m_led = !m_led;
            ledOFF();
        }

        m_liveViewTimer->stop();
        //ui.liveView->Clear();
        ui.liveScanBtn->setText("Live Scan");

        m_liveScanRunning = false;

        delete m_acqusitionThread;
        m_acqusitionThread = nullptr;
    }

    m_acquisitionRunning = false;

    if (runPostProcess) {
        //run post processing steps in new thread
        std::thread postProcessThread([&]() {
            spdlog::info("Starting post processing thread");
            //run images post processing steps
            spdlog::info("calling postProcess");
            postProcess();

            m_userCanceled = false;
            emit sig_progress_done();

            uint16_t rowsxcols = m_config->rows * m_config->cols;
            if (m_config->autoTile && rowsxcols == m_stageControl->GetPositions().size() && rowsxcols == m_config->tileMap.size()) {
                emit sig_start_analysis();
            }
        });

        postProcessThread.detach();
    }

    ui.startAcquisitionBtn->setText("Start Acquisition");
    ui.frameRateEdit->setEnabled(true);
    ui.durationEdit->setEnabled(true);
    ui.ledIntensityEdit->setEnabled(true);

}

/*
 * Signal to indicate the user has modified the settings.
 *
 * @param path The path to save captured images to.
 * @param prefix The file prefix to use for captured images.
 */
void MainWindow::settings_changed(std::filesystem::path path, std::string prefix) {
    spdlog::info("Settings changed, dir: {}, prefix: {}", path.string().c_str(), prefix);
    m_config->path = path;
    m_config->prefix = prefix;

    m_expSettings.workingDir = m_config->path;
    m_expSettings.acquisitionDir = m_config->path;
    m_expSettings.filePrefix = m_config->prefix;
    m_camera->UpdateExp(m_expSettings);
}

/*
 * Signal to indicate the stage position list has changed.
 *
 * @param count The length of the stage position list.
 */
void MainWindow::stagelist_updated(size_t count) {
    availableDriveSpace(m_config->fps, m_config->duration, count);
}


/*
 * Turns on LED with given voltage.
 *
 * @param voltage The value to set analog output voltage to.
 *
 * @return True if successful, false otherwise.
 */
bool MainWindow::ledON(double voltage) {
    const double data[1] = { voltage };
    uint8_t lines[8] = {1,1,1,1,1,1,1,1};
    bool rtnval = true;

    if (!ledSetVoltage(voltage)) {
        spdlog::error("Failed to run taskAO");
    }

   bool taskDO_result = (
        m_DAQmx.StartTask(m_taskDO) && \
        m_DAQmx.WriteDigitalLines(m_taskDO, 1, 0, 10.0, DAQmx_Val_GroupByChannel, lines, NULL) && \
        m_DAQmx.StopTask(m_taskDO)
    );

    if (!taskDO_result) {
        spdlog::error("Failed to run taskDO");
        m_DAQmx.StopTask(m_taskDO);
        rtnval = false;
    }

    spdlog::info("led ON, delaying {}ms", m_config->shutterDelayMs);
    std::this_thread::sleep_for(std::chrono::milliseconds(m_config->shutterDelayMs));

    return rtnval;
}


/*
 * Turns off LED.
 *
 * @return true if successful, false otherwise.
 */
bool MainWindow::ledOFF() {
    spdlog::info("led OFF");
    uint8_t lines[8] = {0,0,0,0,0,0,0,0};
    bool taskDO_result = (
        m_DAQmx.StartTask(m_taskDO) && \
        m_DAQmx.WriteDigitalLines(m_taskDO, 1, 0, 10.0, DAQmx_Val_GroupByChannel, lines, NULL) && \
        m_DAQmx.StopTask(m_taskDO)
    );

    if (!taskDO_result) {
        spdlog::error("Failed to run taskDO");
        return m_DAQmx.StopTask(m_taskDO);
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
        m_DAQmx.StartTask(m_taskAO) && \
        m_DAQmx.WriteAnalogF64(m_taskAO, 1, 0, 10.0, DAQmx_Val_GroupByChannel, data, NULL) && \
        m_DAQmx.StopTask(m_taskAO)
    );
}


void MainWindow::acquire(bool saveToDisk) {
    auto progressCB = [&](size_t n) { emit sig_progress_update(n); };

    if (m_acquisition) {
        spdlog::info("Reusing existing acquistion");
    } else {
        spdlog::info("Creating acquisition");
        m_acquisition = std::make_unique<pmAcquisition>(m_camera);
    }

    spdlog::info("Starting acquisition, live view: {}", !saveToDisk);
    if (!m_acquisition->Start(saveToDisk, progressCB, 0.0, nullptr)) {
        spdlog::error("Failed starting acquisition");
    }

    spdlog::info("Waiting for acquisition");
    m_acquisition->WaitForStop();
}

/*
 * @brief PostProcess acquisition data
 */
void MainWindow::postProcess() {
    if (!m_userCanceled) {
        spdlog::info("Writing settings file to {}\\settings.toml", m_expSettings.acquisitionDir.string());
        //TODO add settingsPath to m_config
        std::filesystem::path settingsPath = m_expSettings.acquisitionDir / "settings.toml";
        std::ofstream outfile(settingsPath.string()); // create output file stream

        std::vector<toml::value> stagePos;
        for (auto& loc : m_stageControl->GetPositions()) {
            stagePos.push_back(toml::value{{"x", loc->x}, {"y", loc->y}});
        }

        //need this here even if auto tile is disabled
        std::string rawFile = fmt::format("{}_{}.raw", m_config->prefix, std::string(m_startAcquisitionTS));

        //output capture settings
        const toml::basic_value<toml::preserve_comments, tsl::ordered_map> settings{
            { "instrument_name", "Nautilus" },
            { "software_version", m_config->version },
            { "recording_date", m_startAcquisitionTS },
            { "led_intensity", m_config->ledIntensity },
            { "output_dir_path", m_expSettings.acquisitionDir.string() },
            { "input_path", (m_expSettings.acquisitionDir / rawFile).string() },
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
            { "xy_pixel_size", m_config->xyPixelSize }
        };

        outfile << std::setw(100) << settings << std::endl;

        //output platemap format
        if (m_config->plateFormat != "") {
            auto platemapFormat = toml::parse(m_config->plateFormat);
            outfile << std::setw(100) << platemapFormat << std::endl;
        }

        outfile.close();

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

            //std::shared_ptr<VideoEncoder> venc = nullptr;
            std::shared_ptr<RawFile<6>> raw = std::make_shared<RawFile<6>>(
                    (m_expSettings.acquisitionDir / rawFile), 16, m_config->cols * m_width, m_config->rows * m_height, m_expSettings.frameCount);

            /* if (m_config->encodeVideo) { */
            /*     std::string aviFile = fmt::format("{}_stack_{}.avi", m_config->prefix, std::string(m_startAcquisitionTS)); */
            /*     std::filesystem::path vidOut = (m_expSettings.acquisitionDir / aviFile); */

            /*     venc = std::make_shared<VideoEncoder>(vidOut, "mpeg4", m_config->fps, m_config->cols*m_width, m_config->rows*m_height); */
            /*     if (!venc->Initialize()) { */
            /*         spdlog::error("Failed to initialize video encoder"); */
            /*     } */
            /* } */

            emit sig_progress_text("Tiling images");
            emit sig_progress_update(1);

            PostProcess::AutoTile(
                m_expSettings.acquisitionDir,
                m_config->prefix,
                m_expSettings.frameCount,
                m_config->rows,
                m_config->cols,
                m_config->tileMap,
                m_width,
                m_height,
                m_config->vflip,
                m_config->hflip,
                !m_config->noAutoConBright,
                [&](size_t n) { emit sig_progress_update(n); },
                raw,
                m_expSettings.storageType
                //venc
            );

            raw->Close();

            if (m_config->encodeVideo) {
                emit sig_start_encoding();
                /* venc->close(); */
            } 
        }
    }
}

/*
 * Thread that starts actual acquisition and waits for acquisition to finish.
 *
 * @param cls Main window object pointer.
 * @param saveToDisk Flag to enable/disable streaming to disk.
 */
void MainWindow::acquisitionThread(MainWindow* cls) {
    bool needPostProcess = false;
    do {
        if (cls->m_liveScanRunning) {
            cls->acquire(false);
        }

        if (cls->m_acquisitionRunning) {
            int pos = 1;
            spdlog::info("Starting acquistions");
            needPostProcess = true;

            if (cls->m_stageControl->GetPositions().empty()) {
                spdlog::info("No stage positions set, adding current position");
                cls->m_stageControl->AddCurrentPosition();
            }

            // get local timestamp to add to subdir name
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::system_clock::to_time_t(now);
            std::tm *tm = std::localtime(&timestamp);

            // make subdirectory to write to
            std::strftime(std::data(cls->m_startAcquisitionTS), std::size(cls->m_startAcquisitionTS), TIMESTAMP_STR, tm);
            std::string subdir = cls->m_config->prefix + std::string(cls->m_startAcquisitionTS);

            cls->m_expSettings.acquisitionDir = cls->m_expSettings.workingDir / subdir;
            if (!std::filesystem::exists(cls->m_expSettings.acquisitionDir)) {
                std::filesystem::create_directories(cls->m_expSettings.acquisitionDir);
                spdlog::info("Acquisition being written under directory: {}", cls->m_expSettings.acquisitionDir.string());
            }

            cls->m_expSettings.expTimeMS = (1 / cls->m_config->fps) * 1000;
            cls->m_expSettings.frameCount = cls->m_config->duration * cls->m_config->fps;

            emit cls->sig_progress_start("Acquiring images", cls->m_stageControl->GetPositions().size() * cls->m_expSettings.frameCount);
            for (auto& loc : cls->m_stageControl->GetPositions()) {
                spdlog::info("Moving stage, x: {}, y: {}", loc->x, loc->y);
                emit cls->sig_progress_text(fmt::format("Acquiring images for position ({}, {})", loc->x, loc->y));
                cls->m_stageControl->SetAbsolutePosition(loc->x, loc->y);

                cls->m_expSettings.filePrefix = fmt::format("{}_{}_", cls->m_config->prefix, pos++);
                cls->m_camera->UpdateExp(cls->m_expSettings);

                cls->acquire(true);
                //TODO check for user cancel and jump out
                if (cls->m_userCanceled) {
                    spdlog::info("User canceled acquisition");
                    break;
                }

                spdlog::info("Acquisition for location x: {}, y: {} finished", loc->x, loc->y);
            }

            cls->m_acquisitionRunning = false;
        }
        spdlog::info("Acquisition done, sending signal");
        emit cls->sig_acquisition_done(needPostProcess);
        //need this so stopping liveview doesn't trigger post processing if liveview was active during capture
        needPostProcess = false;

    } while (cls->m_liveScanRunning);

}


/**
 * Helper function that will take the line time for each mode from the config file and
 * calculate the max frame rate based on the caputure reagion height.
*/
double MainWindow::calcMaxFrameRate(uint16_t p1, uint16_t p2, double line_time) {
    return 1000000.0 / (line_time * abs(p2 - p1));
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

