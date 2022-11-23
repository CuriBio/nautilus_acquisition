#include <spdlog/spdlog.h>
#include <QThread>
#include "mainwindow.h"

MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    connect(this, &MainWindow::sig_acquisition_done, this, &MainWindow::acquisition_done);
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
    if (m_acquisition) {
        std::shared_ptr<pm::Frame> frame = m_acquisition->GetLatestFrame();

        if (frame) {
            ui.liveView->updateImage((uint8_t*)frame->GetData());
        }
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
                    m_acqusitionThread = QThread::create([=] {
                        m_acquisition.reset();
                        m_acquisition = nullptr;

                        spdlog::info("Setup exposure");
                        m_camera->SetupExp(m_expSettings);

                        spdlog::info("Creating acquisition");
                        m_acquisition = std::make_unique<pmAcquisition>(m_camera);

                        spdlog::info("Starting acquisition");
                        if (!m_acquisition->Start(false, 0.0, nullptr)) {
                            spdlog::error("Failed starting acquisition");
                        }

                        spdlog::info("Waiting for acquisition");
                        m_acquisition->WaitForStop();

                        spdlog::info("Acquisition done, sending signal");
                        emit sig_acquisition_done();
                    });
                    m_acqusitionThread->start();
                }
            }
        } else {
            spdlog::info("Stopping acquisition");
            //stop acquisition
            m_acquisition->Abort();
        }
    }
}
