#include <stdlib.h>

#include <spdlog/spdlog.h>
#include <QThread>
#include <QTimer>

#include "mainwindow.h"

MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent) {
    ui.setupUi(this);

    m_path = getenv("USERPROFILE");
    m_prefix = "default_";
    m_settings = new Settings(this, m_path, m_prefix);

    connect(this, &MainWindow::sig_acquisition_done, this, &MainWindow::acquisition_done);
    connect(m_settings, &Settings::sig_settings_changed, this, &MainWindow::settings_changed);

    m_liveViewTimer = new QTimer(this);
    connect(m_liveViewTimer, &QTimer::timeout, this, &MainWindow::updateLiveView);
}

void MainWindow::Initialize() {
    spdlog::info("Initialize camera");
    m_camera = std::make_shared<pmCamera>();

    spdlog::info("Opening camera 0");
    if (!m_camera->Open(0)) {
        //TODO how should the user be notified?
        spdlog::error("Failed to open camera 0");
    }

    spdlog::info("Get camera info");
    m_camInfo = m_camera->GetInfo();

    //set exp region to complete sensor size
    m_expSettings.region = {
        .s1 = 0, .s2 = uns16(m_camInfo.sensorResX - 1), .sbin = 1,
        .p1 = 0, .p2 = uns16(m_camInfo.sensorResY - 1), .pbin = 1
    };

    m_expSettings.filePath = m_path;
    m_expSettings.filePrefix = m_prefix;

    //initial camera setup, need this to allocate buffers
    m_camera->SetupExp(m_expSettings);

    //Set sensor size for live view
    ui.liveView->Init(m_camInfo.sensorResX, m_camInfo.sensorResY);

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
}


/**
 * Slots
 */
void MainWindow::on_ledIntensityEdit_valueChanged(int value) {
    spdlog::info("ledIntensityEdit value changed: {}", value);
}


void MainWindow::on_frameRateEdit_valueChanged(int value) {
    if (m_acquisition && m_acquisition->IsRunning()) {
        spdlog::error("Acquisition running: FPS cannot be changed");
    } else {
        spdlog::info("frameRateEdit value changed: {}", value);
        m_fps = (float)value;
    }
}


void MainWindow::on_durationEdit_valueChanged(double value) {
    spdlog::info("durationEdit value changed: {}", value);
    if(m_acquisition && m_acquisition->IsRunning()) {
        spdlog::error("Acquistion running: duration cannot be changed");
    } else {
        m_duration = value;
    }
}

void MainWindow::on_advancedSetupBtn_clicked() {
    spdlog::info("advancedSetupBtn clicked");
}


void MainWindow::on_liveScanBtn_clicked() {
    spdlog::info("liveScanBtn clicked");
    if (!m_acqusitionThread) {
        spdlog::info("Starting live scan");
        ui.liveScanBtn->setText("Stop Live Scan");

        double minFps = std::min<double>(m_fps, 24.0);
        m_liveViewTimer->start(int32_t(1000 * (1 / minFps)));

        m_acqusitionThread = QThread::create(MainWindow::acquisitionThread, this, false);
        m_acqusitionThread->start();
    } else {
        spdlog::info("Stopping live scan");
        if (m_acquisition) { m_acquisition->Abort(); };
    }
}


void MainWindow::on_settingsBtn_clicked() {
    spdlog::info("settingsBtn clicked");
    m_settings->exec();
}


void MainWindow::on_startAcquisitionBtn_clicked() {
    if (m_duration > 0 && m_fps > 0) {
        m_expSettings.expTimeMS = (1 / m_fps) * 1000;
        m_expSettings.frameCount = uint32_t(m_duration * m_fps);

        if (!m_acquisition || !m_acquisition->IsRunning()) {
            spdlog::info("Starting acquisition: expTimeMS {}, frameCount {}", m_expSettings.expTimeMS, m_expSettings.frameCount);
            ui.startAcquisitionBtn->setText("Stop Acquisition");

            {
                std::unique_lock<std::mutex> lock(m_lock);
                if (!m_acqusitionThread) {
                    m_acqusitionThread = QThread::create(MainWindow::acquisitionThread, this, true);
                    m_acqusitionThread->start();
                } else {
                    spdlog::info("Starting stream to disk");
                    m_acquisition->Start(true, 0.0, nullptr);
                }
            }
        } else if (m_acquisition->GetState() == AcquisitionState::AcqLiveScan) {
            spdlog::info("Starting stream to disk");
            ui.startAcquisitionBtn->setText("Stop Acquisition");
            m_liveViewTimer->stop();
            m_acquisition->Start(true, 0.0, nullptr);
        } else {
            spdlog::info("Stopping acquisition");
            //stop acquisition
            m_acquisition->Abort();
        }
    }
}


void MainWindow::updateLiveView() {
    if (m_acquisition) {
        pm::Frame* frame = m_acquisition->GetLatestFrame();

        if (frame) {
            ui.liveView->updateImage((uint8_t*)frame->GetData());
        }
    }
}

/**
 * Signals
 */
void MainWindow::acquisition_done() {
    spdlog::info("Acquisition done signal");
    m_liveViewTimer->stop();
    ui.liveView->clear();

    ui.startAcquisitionBtn->setText("Start Acquisition");
    ui.liveScanBtn->setText("Live Scan");
    delete m_acqusitionThread;
    m_acqusitionThread = nullptr;
}


void MainWindow::settings_changed(std::filesystem::path path, std::string prefix) {
    spdlog::info("Settings changed, dir: {}, prefix: {}", path.string().c_str(), prefix);
    m_path = path;
    m_prefix = prefix;

    m_expSettings.filePath = m_path;
    m_expSettings.filePrefix = m_prefix;
}

/**
 * Threads
 */
void MainWindow::acquisitionThread(MainWindow* cls, bool saveToDisk) {
    if (cls->m_acquisition) {
        spdlog::info("Reusing existing acquistion");
        //cls->m_acquisition = nullptr;
    } else {
        spdlog::info("Creating acquisition");
        cls->m_acquisition = std::make_unique<pmAcquisition>(cls->m_camera);
    }

    cls->m_expSettings.expTimeMS = (1 / cls->m_fps) * 1000;
    cls->m_expSettings.frameCount = cls->m_duration * cls->m_fps;

    spdlog::info("Setup exposure");
    cls->m_camera->SetupExp(cls->m_expSettings);

    spdlog::info("Starting acquisition");
    if (!cls->m_acquisition->Start(saveToDisk, 0.0, nullptr)) {
        spdlog::error("Failed starting acquisition");
    }

    spdlog::info("Waiting for acquisition");
    cls->m_acquisition->WaitForStop();

    spdlog::info("Acquisition done, sending signal");
    emit cls->sig_acquisition_done();
}
