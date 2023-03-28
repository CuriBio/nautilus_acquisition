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

#include <stdlib.h>
#include <format>
#include <iostream>
#include <ctime>
#include <chrono>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#include <fileapi.h>
#endif

#include <spdlog/spdlog.h>
#include <QMessageBox>
#include <QThread>
#include <QTimer>

#include "mainwindow.h"


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
MainWindow::MainWindow(
    std::string version,
    std::string path,
    std::string prefix,
    std::string niDev,
    std::string testImgPath,
    double fps,
    double duration,
    double expTimeMs,
    uint16_t spdtable,
    double ledIntensity,
    uint32_t bufferCount,
    uint32_t frameCount,
    StorageType storageType,
    uint16_t triggerMode,
    uint16_t exposureMode,
    double maxVoltage,
    bool autoConBright,
    bool vflip, bool hflip,
    Region& rgn,
    std::string stageComPort,
    std::string configFile,
    toml::value& config,
    double line_times[4],
    QMainWindow *parent) : QMainWindow(parent)
{
    ui.setupUi(this);


    m_line_times[0] = line_times[0];
    m_line_times[1] = line_times[1];
    m_line_times[2] = line_times[2];
    m_line_times[3] = line_times[3];

    m_version = version;

    m_path = path;
    m_prefix = prefix;
    m_niDev = niDev;
    m_testImgPath = testImgPath;
    m_autoConBright = autoConBright;
    m_vflip = vflip;
    m_hflip = hflip;
    m_stageComPort = stageComPort;

    m_settings = new Settings(this, m_path, m_prefix);
    m_stageControl = new StageControl(m_stageComPort,configFile, this);

    m_duration = duration;
    m_fps = fps;
    m_expTimeMS = expTimeMs;
    m_spdtable = spdtable;
    m_maxVoltage = maxVoltage;
    m_ledIntensity = ledIntensity;
    m_config = config;
    m_advancedSettingsDialog = new AdvancedSetupDialog(m_config,this);

    m_width = (rgn.s2 - rgn.s1 + 1) / rgn.sbin;
    m_height = (rgn.p2 - rgn.p1 + 1) / rgn.pbin;

    m_expSettings.spdTableIdx = spdtable;
    m_expSettings.expTimeMS = expTimeMs,
    m_expSettings.frameCount = frameCount;
    m_expSettings.bufferCount = bufferCount;
    m_expSettings.storageType = storageType;
    m_expSettings.trigMode = triggerMode;
    m_expSettings.expModeOut = exposureMode;
    m_expSettings.region = {
        .s1 = uns16(rgn.s1), .s2 = uns16(rgn.s2), .sbin = rgn.sbin,
        .p1 = uns16(rgn.p1), .p2 = uns16(rgn.p2), .pbin = rgn.pbin
    };

    connect(this, &MainWindow::sig_acquisition_done, this, &MainWindow::acquisition_done);
    connect(m_settings, &Settings::sig_settings_changed, this, &MainWindow::settings_changed);
    connect(m_advancedSettingsDialog,&AdvancedSetupDialog::sig_ni_dev_change,this,&MainWindow::Resetup_ni_device);

    m_liveViewTimer = new QTimer(this);
    connect(m_liveViewTimer, &QTimer::timeout, this, &MainWindow::updateLiveView);

    m_hist = new uint32_t[(1<<16) - 1];
    memset((void*)m_hist, 0, sizeof(uint32_t)*((1<<16)-1));

    m_lut16 = new uint8_t[(1<<16) - 1];
    memset((void*)m_lut16, 0, (1<<16)-1);

    m_taskFrameStats = std::make_shared<TaskFrameStats>(TASKS);
    m_taskUpdateLut = std::make_shared<TaskFrameLut16>();
    m_taskApplyLut = std::make_shared<TaskApplyLut16>();
    std::vector<std::string> devicelist = m_DAQmx.GetListOfDevices();
    m_advancedSettingsDialog->Initialize(devicelist);
}


/*
 * Initializes main window and camera/acquisition objects.
 */
void MainWindow::Initialize() {
    spdlog::info("Initialize camera");
    m_camera = std::make_shared<pmCamera>();

    spdlog::info("Opening camera 0");
    if (!m_camera->Open(0)) {
        //TODO how should the user be notified?
        spdlog::error("Failed to open camera 0");

        QMessageBox messageBox;
        messageBox.critical(0,"Error","Camera could not be found, please plug in camera and restart application");
        messageBox.setFixedSize(500,200);
        exit(1);
    }

    spdlog::info("Get camera info");
    m_camInfo = m_camera->GetInfo();

    m_expSettings.workingDir = m_path;
    m_expSettings.acquisitionDir = m_path;
    m_expSettings.filePrefix = m_prefix;

    //initial camera setup, need this to allocate buffers
    m_camera->SetupExp(m_expSettings);

    //for 8 bit image conversion for liveview, might not need it anymore
    m_img8 = new uint8_t[m_width*m_height];

    //Set sensor size for live view
    ui.liveView->Init(m_width, m_height, m_vflip, m_hflip, ImageFormat::Mono8);
    ui.histView->Init(m_hist, m_width*m_height);

    spdlog::info("Setting region: (s1: {}, s2: {}, p1: {}, p2: {}, sbin: {}, pbin: {}",
        m_expSettings.region.s1,
        m_expSettings.region.s2,
        m_expSettings.region.p1,
        m_expSettings.region.p2,
        m_expSettings.region.sbin,
        m_expSettings.region.pbin
    );
    spdlog::info("Image capture width: {}, height: {}", m_width, m_height);

    //log speed table
    spdlog::info("Speed Table:");
    for(auto& i : m_camInfo.spdTable) {
        spdlog::info("\tport: {}, pixTimeNs: {}, spdIndex: {}, gainIndex: {}, gainName: {}, bitDepth: {}", i.portName, i.pixTimeNs, i.spdIndex, i.gainIndex, i.gainName, i.bitDepth);
    }

    //Set max Frame rate
    int max_frame_rate = calc_max_frame_rate(m_expSettings.region.p1,m_expSettings.region.p2,m_camInfo.spdTable[0].spdIndex,m_line_times,m_spdtable);

    //needs camera to be opened first
    m_acquisition = std::make_unique<pmAcquisition>(m_camera);
    if (m_testImgPath != "") {
        m_acquisition->LoadTestData(m_testImgPath);
    }

    //Setup NIDAQmx controller for LED
    m_taskAO = "Analog_Out_Volts"; //Task for setting Analog Output voltage
    m_devAO = fmt::format("{}/ao0", m_niDev); //Device name for analog output
    spdlog::info("Using NI device {} for analog output", m_devAO);

    m_taskDO = "Digital_Out"; //Task for setting Digital Output
    m_devDO = fmt::format("{}/port0/line0:7", m_niDev); //Device for digital output
    spdlog::info("Using NI device {} for digital output", m_devDO);

    m_DAQmx.CreateTask(m_taskAO);
    m_DAQmx.CreateTask(m_taskDO);

    m_DAQmx.CreateAnalogOutpuVoltageChan(m_taskAO, m_devAO.c_str(), -10.0, 10.0, DAQmx_Val_Volts);
    m_DAQmx.CreateDigitalOutputChan(m_taskDO, m_devDO.c_str(), DAQmx_Val_ChanForAllLines);

    ui.ledIntensityEdit->setValue(m_ledIntensity);
    ui.frameRateEdit->setValue(max_frame_rate);
    ui.durationEdit->setValue(m_duration);
}


/*
* Runs when a new ni device is selected, re configure ni device leds
*/
void MainWindow::Resetup_ni_device(std::string new_m_niDev){
    m_niDev = new_m_niDev;
    m_DAQmx.ClearTask(m_taskAO);
    m_DAQmx.ClearTask(m_taskDO);
    //Setup NIDAQmx controller for LED
    m_taskAO = "new_Analog_Out_Volts"; //Task for setting Analog Output voltage
    m_devAO = fmt::format("{}/ao0", m_niDev); //Device name for analog output
    spdlog::info("Using NI device {} for analog output", m_devAO);

    m_taskDO = "new_Digital_Out"; //Task for setting Digital Output
    m_devDO = fmt::format("{}/port0/line0:7", m_niDev); //Device for digital output
    spdlog::info("Using NI device {} for digital output", m_devDO);

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
    m_ledIntensity = value;
    double voltage = (m_ledIntensity / 100.0) * m_maxVoltage;
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
bool MainWindow::available_space_in_default_drive( double fps,double duration){
#ifdef _WIN32
    if (m_camera->ctx) {
        uns32 frameBytes = m_camera->ctx->frameBytes;
        ULARGE_INTEGER  lpTotalNumberOfFreeBytes = {0};
        std::stringstream tool_tip_text;

        if (!GetDiskFreeSpaceEx(m_path.c_str(), nullptr, nullptr, &lpTotalNumberOfFreeBytes)) {
            //default drive could not be found
            spdlog::error("Default drive could not be found when");
            tool_tip_text << "Please check device is properly pulgged into " << m_path << " drive.";
            ui.startAcquisitionBtn->setToolTip(QString::fromStdString(tool_tip_text.str()));
            return false;
        } else if (lpTotalNumberOfFreeBytes.QuadPart > fps * duration * frameBytes) {
            //space for acquisition found
            std::stringstream storage_space_string,driver_name_string;
            storage_space_string << lpTotalNumberOfFreeBytes.QuadPart;
            driver_name_string << m_path;
            spdlog::info("Drive {} has: {} bytes free for acquisition",driver_name_string.str(),storage_space_string.str());
            return true;
        } else {
            //not enough space for acquisition
            spdlog::error("Not enough space for acquisition");
            tool_tip_text << "Not enough space in " << m_path << " drive.";
            ui.startAcquisitionBtn->setToolTip(QString::fromStdString(tool_tip_text.str()));
            return false;
        }
    } else {
        spdlog::error("Camera context could not be found when checking for space in default drive");
        ui.startAcquisitionBtn->setToolTip("Camera can not be found.");
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
    if (value * m_duration < 1.0) {
        spdlog::error("Capture is set to less than 1 frame, fps: {}, duration: {}", value, m_duration);
        ui.frameRateEdit->setStyleSheet("background-color: red");
        ui.durationEdit->setStyleSheet("background-color: red");
    } else if (!available_space_in_default_drive(value,m_duration)){
        ui.startAcquisitionBtn->setStyleSheet("background-color: gray");
    } else {
        ui.frameRateEdit->setStyleSheet("background-color: white");
        ui.durationEdit->setStyleSheet("background-color: white");
        ui.startAcquisitionBtn->setStyleSheet("");
        ui.startAcquisitionBtn->setToolTip("");

        spdlog::info("Setting new exposure value");
        m_fps = value;
        m_expSettings.expTimeMS = (1 / m_fps) * 1000;
        m_expSettings.frameCount = m_duration * m_fps;
    }
}


/*
 * Duration edit box slot, called when users changes duration value.
 *
 * @param value The updated duration value in seconds.
 */
void MainWindow::on_durationEdit_valueChanged(double value) {
    if (value * m_fps < 1.0) {
        spdlog::error("Capture is set to less than 1 frame, fps: {}, duration: {}", value, m_duration);
        ui.frameRateEdit->setStyleSheet("background-color: red");
        ui.durationEdit->setStyleSheet("background-color: red");
    }else if (!available_space_in_default_drive(value,m_duration)){
        ui.startAcquisitionBtn->setStyleSheet("background-color: gray");
    }else {
        ui.frameRateEdit->setStyleSheet("background-color: white");
        ui.durationEdit->setStyleSheet("background-color: white");
        ui.startAcquisitionBtn->setStyleSheet("");
        ui.startAcquisitionBtn->setToolTip("");
    }
    m_duration = value;
    m_expSettings.expTimeMS = (1 / m_fps) * 1000;
    m_expSettings.frameCount = m_duration * m_fps;
}


/*
 * Advanced setup button slot, called when user clicks on advanced setup button.
 */
void MainWindow::on_advancedSetupBtn_clicked() {
    spdlog::info("Showing advanced setup window");
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

        // max frame rate allowed in live scan is 24, acquisition can capture at higher frame rates
        double minFps = std::min<double>(m_fps, 24.0);
        m_liveViewTimer->start(int32_t(1000 * (1.0 / minFps)));

        if (!m_acquisitionRunning) {
            StartAcquisition(false);
        }
        ui.liveScanBtn->setText("Stop Live Scan");
    } else {
        spdlog::info("Stopping live scan");
        m_liveScanRunning = false;

        if (!m_acquisitionRunning) {
            // this will also stop the live view timer
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
        StartAcquisition(true);
    } else {
        m_acquisitionRunning = false;
        if (!m_liveScanRunning) {
            StopAcquisition();
        }
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

    if (m_duration > 0 && m_fps > 0) {
        if (!m_acquisition) {
            spdlog::error("m_acquisition is invalid");
            return;
        }

        if (!m_led && m_ledIntensity > 0.0) {
            m_led = !m_led;
            double voltage = (m_ledIntensity / 100.0) * m_maxVoltage;
            spdlog::info("Setting led intensity {}, voltage {}, max voltage {}", m_ledIntensity, voltage, m_maxVoltage);
            ledON(voltage);
        }

        switch (m_acquisition->GetState()) {
            case AcquisitionState::AcqStopped:
                {
                    spdlog::info("StartAcquisition, current state AcqStopped");

                    double expTimeMs = (1.0 / m_fps) * 1000;
                    spdlog::info("Setting expTimeMS: {} ({})", static_cast<uint32_t>(expTimeMs), expTimeMs);

                    m_expSettings.expTimeMS = static_cast<uint32_t>(expTimeMs);
                    m_expSettings.frameCount = uint32_t(m_duration * m_fps);
                    spdlog::info("Starting acquisition: expTimeMS {}, frameCount {}", m_expSettings.expTimeMS, m_expSettings.frameCount);

                    if (!m_acqusitionThread) {
                        m_acqusitionThread = QThread::create(MainWindow::acquisitionThread, this);
                        m_acqusitionThread->start();
                    } else {
                        m_acquisition->Start(saveToDisk, 0.0, nullptr);
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
    }
}


/*
 * Stops a running acquisition and turns off LED.
 */
void MainWindow::StopAcquisition() {
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
                m_acquisition->Start(false, 0.0, nullptr);
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
            AutoConBright(data);

            ui.liveView->UpdateImage((uint8_t*)m_img8);
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
    //Calculate histogram
    m_taskFrameStats->Setup(data, m_hist, m_width, m_height);
    m_parTask.Start(m_taskFrameStats);
    m_taskFrameStats->Results(m_min, m_max, m_hmax);

    if (m_autoConBright) {
        /* //Update lut */
        m_taskUpdateLut->Setup(m_min, m_max);
        m_parTask.Start(m_taskUpdateLut);
        uint8_t* lut = m_taskUpdateLut->Results();

        //Apply lut
        m_taskApplyLut->Setup(data, m_img8, lut, m_width * m_height);
        m_parTask.Start(m_taskApplyLut);
    }
}


/*
 * Signal to indicate acquisition has finished.
 */
void MainWindow::acquisition_done() {
    spdlog::info("Acquisition done signal");
    std::unique_lock<std::mutex> lock(m_lock);

    if (m_led) {
        m_led = !m_led;
        ledOFF();
    }

    m_liveViewTimer->stop();
    //ui.liveView->Clear();


    m_acquisitionRunning = false;
    m_liveScanRunning = false;


    ui.startAcquisitionBtn->setText("Start Acquisition");
    ui.liveScanBtn->setText("Live Scan");

    delete m_acqusitionThread;
    m_acqusitionThread = nullptr;
}

/*
 * Signal to indicate the user has modified the settings.
 *
 * @param path The path to save captured images to.
 * @param prefix The file prefix to use for captured images.
 */
void MainWindow::settings_changed(std::filesystem::path path, std::string prefix) {
    spdlog::info("Settings changed, dir: {}, prefix: {}", path.string().c_str(), prefix);
    m_path = path;
    m_prefix = prefix;

    m_expSettings.workingDir = m_path;
    m_expSettings.acquisitionDir = m_path;
    m_expSettings.filePrefix = m_prefix;
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

    spdlog::info("led ON");
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
    if (m_acquisition) {
        spdlog::info("Reusing existing acquistion");
    } else {
        spdlog::info("Creating acquisition");
        m_acquisition = std::make_unique<pmAcquisition>(m_camera);
    }

    spdlog::info("Starting acquisition, live view: {}", !saveToDisk);
    if (!m_acquisition->Start(saveToDisk, 0.0, nullptr)) {
        spdlog::error("Failed starting acquisition");
    }

    spdlog::info("Waiting for acquisition");
    m_acquisition->WaitForStop();
}


/*
 * Thread that starts actual acquisition and waits for acquisition to finish.
 *
 * @param cls Main window object pointer.
 * @param saveToDisk Flag to enable/disable streaming to disk.
 */
void MainWindow::acquisitionThread(MainWindow* cls) {
    do {
        if (cls->m_liveScanRunning) {
            cls->acquire(false);
        }

        if (cls->m_acquisitionRunning) {
            int pos = 1;
            spdlog::info("Starting acquistions");

            if (cls->m_stageControl->GetPositions().empty()) {
                spdlog::info("No stage positions set, adding current position");
                cls->m_stageControl->AddCurrentPosition();
            }

            // get local timestamp to add to subdir name
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::system_clock::to_time_t(now);

            // make subdirectory to write to
            std::tm *tm = std::localtime(&timestamp);
            char buffer[20];
            std::strftime(buffer, 20, "%Y_%m_%d_%H%M%S", tm);
            std::string subdir = cls->m_prefix + std::string(buffer);

            cls->m_expSettings.acquisitionDir = cls->m_expSettings.workingDir / subdir;
            if (!std::filesystem::exists(cls->m_expSettings.acquisitionDir)) {
                std::filesystem::create_directories(cls->m_expSettings.acquisitionDir);
                spdlog::info("Acquisition being written under directory: {}", cls->m_expSettings.acquisitionDir.string());
            }

            cls->m_expSettings.expTimeMS = (1 / cls->m_fps) * 1000;
            cls->m_expSettings.frameCount = cls->m_duration * cls->m_fps;

            for (auto& loc : cls->m_stageControl->GetPositions()) {
                spdlog::info("Moving stage, x: {}, y: {}", loc->x, loc->y);
                cls->m_stageControl->SetAbsolutePosition(loc->x, loc->y);

                cls->m_expSettings.filePrefix = fmt::format("{}_{}_", cls->m_prefix, pos++);
                cls->m_camera->UpdateExp(cls->m_expSettings);

                cls->acquire(true);
                spdlog::info("Acquisition for location x: {}, y: {} finished", loc->x, loc->y);
            }

            spdlog::info("Writing settings file to {}\\settings.toml", cls->m_expSettings.acquisitionDir.string());
            std::filesystem::path settingsPath = cls->m_expSettings.acquisitionDir / "settings.toml";
            std::ofstream outfile(settingsPath.string()); // create output file stream

            std::vector<toml::value> stagePos;
            for (auto& loc : cls->m_stageControl->GetPositions()) {
                stagePos.push_back(toml::value{{"x", loc->x}, {"y", loc->y}});
            }

            const toml::value settings{
                {"software_version", cls->m_version},
                {"led_intensity", cls->m_ledIntensity},
                {"fps", cls->m_fps},
                {"duration", cls->m_expSettings.expTimeMS},
                {"frame_count", cls->m_expSettings.frameCount},
                {"vflip", cls->m_vflip},
                {"hflip", cls->m_hflip},
                {"stage_positions", stagePos}
            };

            outfile << std::setw(0) << settings << std::endl;
            outfile.close();

            cls->m_acquisitionRunning = false;
            cls->ui.startAcquisitionBtn->setText("Start Acquisition");
        }
    } while (cls->m_liveScanRunning);

    spdlog::info("Acquisition done, sending signal");
    emit cls->sig_acquisition_done();
}



/**
 * Helper function that will take the line time for each mode from the config file and
 * calculate the max frame rate based on the caputure reagion height.
*/
int MainWindow::calc_max_frame_rate(int p1,int p2,int16_t spdtable_index,double line_times [4],uint16_t spdtable){
    spdlog::info("Calculating max frame rate");


    int number_of_rows = int(abs(p2 - p1));
    double max_frame_rate = .95 * 3200 * line_times[spdtable] / number_of_rows;

    spdlog::info("Max frame rate is: {},",max_frame_rate);
    return max_frame_rate;
}

