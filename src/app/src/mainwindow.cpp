#include <spdlog/spdlog.h>
#include <QThread>
#include "mainwindow.h"

MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    connect(this, &MainWindow::sig_acquisition_done, this, &MainWindow::acquisition_done);
    connect(this, &MainWindow::sig_livescan_stopped, this, &MainWindow::livescan_stopped);
}

void MainWindow::Initialize() {
    spdlog::info("Initialize camera");

    m_camera = std::make_shared<pmCamera>();

    spdlog::info("Opening camera 0");
    if (!m_camera->Open(0)) {
        spdlog::error("Failed to open camera 0");
    }

    spdlog::info("Get camera info");
    m_camInfo = m_camera->GetInfo();

    m_expSettings.region = {
        .s1 = 0, .s2 = uns16(m_camInfo.sensorResX - 1), .sbin = 1,
        .p1 = 0, .p2 = uns16(m_camInfo.sensorResY - 1), .pbin = 1
    };

    spdlog::info("Setting region: (s1: {}, s2: {}, p1: {}, p2: {}, sbin: {}, pbin: {}",
        m_expSettings.region.s1,
        m_expSettings.region.s2,
        m_expSettings.region.p1,
        m_expSettings.region.p2,
        m_expSettings.region.sbin,
        m_expSettings.region.pbin
    );

    spdlog::info("Speed Table:");
    for(auto& i : m_camInfo.spdTable) {
        spdlog::info("\tport: {}, pixTimeNs: {}, spdIndex: {}, gainIndex: {}, gainName: {}, bitDepth: {}", i.portName, i.pixTimeNs, i.spdIndex, i.gainIndex, i.gainName, i.bitDepth);
    }
}

/**
 * Slots/Signals
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

void MainWindow::on_durationEdit_valueChanged(int value) {
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
    if (!m_liveViewThread && !m_acqusitionThread) {
        spdlog::info("Starting live scan");
        ui.liveScanBtn->setText("Stop Live Scan");
        m_stopLiveView = false;

        m_liveViewThread = QThread::create(MainWindow::liveViewThreadFn, this);
        m_liveViewThread->start();

        m_acqusitionThread = QThread::create(MainWindow::acquisitionThread, this, false);
        m_acqusitionThread->start();
    } else if(m_acquisition && m_acquisition->IsRunning()) {
        spdlog::info("Stopping live scan");
        m_acquisition->Abort();
        m_stopLiveView = true;
    }
}

void MainWindow::on_settingsBtn_clicked() {
    spdlog::info("settingsBtn clicked");
}

void MainWindow::acquisition_done() {
    spdlog::info("Acquisition done signal");
    ui.startAcquisitionBtn->setText("Start Acquisition");
    delete m_acqusitionThread;
    m_acqusitionThread = nullptr;
}

void MainWindow::livescan_stopped() {
    spdlog::info("Live scan stopped");
    ui.liveScanBtn->setText("Start Live Scan");

    delete m_liveViewThread;
    m_liveViewThread = nullptr;
    m_stopLiveView = true;

    ui.liveView->clear();
}

void MainWindow::on_startAcquisitionBtn_clicked() {
    if (m_duration > 0 && m_fps > 0) {
        m_expSettings.expTimeMS = (1 / m_fps) * 1000;
        m_expSettings.frameCount = m_duration * m_fps;

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
            m_acquisition->Start(true, 0.0, nullptr);
        } else {
            spdlog::info("Stopping acquisition");
            //stop acquisition
            m_acquisition->Abort();
        }
    }
}

/**
 * Threads
 */
void MainWindow::acquisitionThread(MainWindow* cls, bool saveToDisk) {
    if (cls->m_acquisition) {
        cls->m_acquisition.reset();
        cls->m_acquisition = nullptr;
    }

    cls->m_expSettings.expTimeMS = (1 / cls->m_fps) * 1000;
    cls->m_expSettings.frameCount = cls->m_duration * cls->m_fps;

    spdlog::info("Setup exposure");
    cls->m_camera->SetupExp(cls->m_expSettings);

    spdlog::info("Creating acquisition");
    cls->m_acquisition = std::make_unique<pmAcquisition>(cls->m_camera);

    spdlog::info("Starting acquisition");
    if (!cls->m_acquisition->Start(saveToDisk, 0.0, nullptr)) {
        spdlog::error("Failed starting acquisition");
    }

    spdlog::info("Waiting for acquisition");
    cls->m_acquisition->WaitForStop();

    spdlog::info("Acquisition done, sending signal");
    emit cls->sig_acquisition_done();
}

void MainWindow::liveViewThreadFn(MainWindow* cls) {
    float minFps = std::min<float>(cls->m_fps, 24.0);
    uint32_t quiescence = uint32_t(1000 * (1 / minFps));

    while (!cls->m_stopLiveView) {
        QThread::msleep(quiescence);
        if (cls->m_acquisition) {
            std::shared_ptr<pm::Frame> frame = cls->m_acquisition->GetLatestFrame();

            if (frame) {
                cls->ui.liveView->updateImage((uint8_t*)frame->GetData());
            }
        }
    }

    emit cls->sig_livescan_stopped();
}

