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
#include <stdlib.h>
#include <format>

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
    std::string configFile,
    toml::value& config,
    QMainWindow *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    m_path = path;
    m_prefix = prefix;
    m_niDev = niDev;
    m_testImgPath = testImgPath;
    m_autoConBright = autoConBright;

    m_settings = new Settings(this, m_path, m_prefix);
    m_stageControl = new StageControl(this);

    m_duration = duration;
    m_fps = fps;
    m_expTimeMS = expTimeMs;
    m_spdtable = spdtable;
    m_maxVoltage = maxVoltage;
    m_ledIntensity = ledIntensity;
    m_config = config;
    m_configFile = configFile;

    m_expSettings.spdTableIdx = spdtable;
    m_expSettings.expTimeMS = expTimeMs,
    m_expSettings.frameCount = frameCount;
    m_expSettings.bufferCount = bufferCount;
    m_expSettings.storageType = storageType;
    m_expSettings.trigMode = triggerMode;
    m_expSettings.expModeOut = exposureMode;

    connect(this, &MainWindow::sig_acquisition_done, this, &MainWindow::acquisition_done);
    connect(m_settings, &Settings::sig_settings_changed, this, &MainWindow::settings_changed);

    m_liveViewTimer = new QTimer(this);
    connect(m_liveViewTimer, &QTimer::timeout, this, &MainWindow::updateLiveView);

    m_hist = new uint32_t[(1<<16) - 1];
    memset((void*)m_hist, 0, sizeof(uint32_t)*((1<<16)-1));

    m_lut16 = new uint8_t[(1<<16) - 1];
    memset((void*)m_lut16, 0, (1<<16)-1);

    m_taskFrameStats = std::make_shared<TaskFrameStats>(TASKS);
    m_taskUpdateLut = std::make_shared<TaskFrameLut16>();
    m_taskApplyLut = std::make_shared<TaskApplyLut16>();
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

    //Using default dimensions of 1600x1200 for now
    m_width = 1600; //m_camInfo.sensorResX;
    m_height = 1200; //m_camInfo.sensorResY;

    //set exp region to complete sensor size
    m_expSettings.region = {
        /* .s1 = 0, .s2 = uns16(m_camInfo.sensorResX - 1), .sbin = 1, */
        /* .p1 = 0, .p2 = uns16(m_camInfo.sensorResY - 1), .pbin = 1 */

        //Default to coords 800, 1000 for now
        .s1 = uns16(800), .s2 = uns16(800 + m_width - 1), .sbin = 1,
        .p1 = uns16(1000), .p2 = uns16(1000 + m_height - 1), .pbin = 1
    };

    m_expSettings.filePath = m_path;
    m_expSettings.filePrefix = m_prefix;

    //initial camera setup, need this to allocate buffers
    m_camera->SetupExp(m_expSettings);

    //for 8 bit image conversion for liveview, might not need it anymore
    m_img8 = new uint8_t[m_width*m_height];

    //Set sensor size for live view
    ui.liveView->Init(m_width, m_height, ImageFormat::Mono8);
    ui.histView->Init(m_hist, m_width*m_height);

    spdlog::info("Setting region: (s1: {}, s2: {}, p1: {}, p2: {}, sbin: {}, pbin: {}",
        m_expSettings.region.s1,
        m_expSettings.region.s2,
        m_expSettings.region.p1,
        m_expSettings.region.p2,
        m_expSettings.region.sbin,
        m_expSettings.region.pbin
    );

    //log speed table
    spdlog::info("Speed Table:");
    for(auto& i : m_camInfo.spdTable) {
        spdlog::info("\tport: {}, pixTimeNs: {}, spdIndex: {}, gainIndex: {}, gainName: {}, bitDepth: {}", i.portName, i.pixTimeNs, i.spdIndex, i.gainIndex, i.gainName, i.bitDepth);
    }

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
    ui.frameRateEdit->setValue(m_fps);
    ui.durationEdit->setValue(m_duration);
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
 * Frame Rate edit box slot, called when users changes the FPS value.
 *
 * @param value The updated FPS value.
 */
void MainWindow::on_frameRateEdit_valueChanged(double value) {
    if (m_acquisition && m_acquisition->IsRunning()) {
        spdlog::error("Acquisition running: FPS cannot be changed");
    } else {
        if (value * m_duration < 1.0) {
            spdlog::error("Capture is set to less than 1 frame, fps: {}, duration: {}", value, m_duration);
            ui.frameRateEdit->setStyleSheet("background-color: red");
            ui.durationEdit->setStyleSheet("background-color: red");
        } else {
            ui.frameRateEdit->setStyleSheet("background-color: white");
            ui.durationEdit->setStyleSheet("background-color: white");
        }
        m_fps = value;
    }
}


/*
 * Duration edit box slot, called when users changes duration value.
 *
 * @param value The updated duration value in seconds.
 */
void MainWindow::on_durationEdit_valueChanged(double value) {
    //spdlog::info("durationEdit value changed: {}", value);
    if(m_acquisition && m_acquisition->IsRunning()) {
        spdlog::error("Acquistion running: duration cannot be changed");
    } else {
        if (value * m_fps < 1.0) {
            spdlog::error("Capture is set to less than 1 frame, fps: {}, duration: {}", value, m_duration);
            ui.frameRateEdit->setStyleSheet("background-color: red");
            ui.durationEdit->setStyleSheet("background-color: red");
        } else {
            ui.frameRateEdit->setStyleSheet("background-color: white");
            ui.durationEdit->setStyleSheet("background-color: white");
        }
        m_duration = value;
    }
}


/*
 * Advanced setup button slot, called when user clicks on advanced setup button.
 * Currently does nothing.
 */
void MainWindow::on_advancedSetupBtn_clicked() {
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
    if (!m_liveScanRunning && !m_acquisitionRunning) {
        spdlog::info("Starting live scan");
        m_liveScanRunning = true;

        double minFps = std::min<double>(m_fps, 24.0);
        m_liveViewTimer->start(int32_t(1000 * (1.0 / minFps)));
        StartAcquisition(false);

        ui.liveScanBtn->setText("Stop Live Scan");
    } else if (!m_liveScanRunning && m_acquisitionRunning) {
        double minFps = std::min<double>(m_fps, 24.0);
        m_liveViewTimer->start(int32_t(1000 * (1.0 / minFps)));
        m_liveScanRunning = true;
    } else if (m_liveScanRunning && m_acquisitionRunning) {
        spdlog::info("Stopping live scan");
        m_liveScanRunning = false;
        m_liveViewTimer->stop();
    } else if (m_liveScanRunning && !m_acquisitionRunning) {
        spdlog::info("Stopping live scan");
        m_liveScanRunning = false;
        StopAcquisition();
    }
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

        if (!m_led) {
            m_led = !m_led;
            double voltage = (m_ledIntensity / 100.0) * m_maxVoltage;
            spdlog::info("Setting led intensity {}, voltage {}, max voltage {}", m_ledIntensity, voltage, m_maxVoltage);
            ledON(voltage);
        }

        /* m_acquisitionRunning = saveToDisk; */
        /* m_liveScanRunning = !saveToDisk; */

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

    m_expSettings.filePath = m_path;
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


void MainWindow::acquire(bool saveToDisk, std::string prefix) {
    if (m_acquisition) {
        spdlog::info("Reusing existing acquistion");
    } else {
        spdlog::info("Creating acquisition");
        m_acquisition = std::make_unique<pmAcquisition>(m_camera);
    }

    m_expSettings.expTimeMS = (1 / m_fps) * 1000;
    m_expSettings.frameCount = m_duration * m_fps;

    spdlog::info("Setup exposure");
    m_expSettings.filePrefix = fmt::format("{}_", prefix);
    m_camera->SetupExp(m_expSettings);

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
            cls->acquire(false, cls->m_prefix);
        }

        if (cls->m_acquisitionRunning) {
            int pos = 1;
            spdlog::info("Starting acquistions");
            for (auto& loc : cls->m_stageControl->GetPositions()) {
                spdlog::info("Moving stage, x: {}, y: {}", loc->x, loc->y);
                cls->m_stageControl->SetAbsolutePosition(loc->x, loc->y);
                std::string prefix = fmt::format("{}_{}_", cls->m_prefix, pos++);

                cls->acquire(true, prefix);
                spdlog::info("Acquisition for location x: {}, y: {} finished", loc->x, loc->y);
            }
            cls->m_acquisitionRunning = false;
            cls->ui.startAcquisitionBtn->setText("Start Acquisition");
        }
    } while (cls->m_liveScanRunning);

    spdlog::info("Acquisition done, sending signal");
    emit cls->sig_acquisition_done();
}
