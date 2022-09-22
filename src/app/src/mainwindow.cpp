#include <spdlog/spdlog.h>

#include "mainwindow.h"

MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent) {
    ui.setupUi(this);
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

    m_camera->SetupExp(m_expSettings);
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
        m_expSettings.expTimeMS = (1 / value) * 1000;
        m_camera->SetupExp(m_expSettings);
    }
}

void MainWindow::on_durationEdit_valueChanged(int value) {
    spdlog::info("durationEdit value changed: {}", value);
}

void MainWindow::on_advancedSetupBtn_clicked() {
    spdlog::info("advancedSetupBtn clicked");
}

void MainWindow::on_liveScanBtn_clicked() {
    spdlog::info("liveScanBtn clicked");
}

void MainWindow::on_settingsBtn_clicked() {
    spdlog::info("settingsBtn clicked");
}

void MainWindow::on_startAcquisitionBtn_clicked() {
    spdlog::info("Start Acquisition clicked");
    if (!m_acquisition) {
        spdlog::info("Creating acquisition");
        m_acquisition = std::make_shared<pmAcquisition>(m_camera);
    }

    if (!m_acquisition->IsRunning()) {
        spdlog::info("Starting acquisition");
        m_acquisition->Start(0.0, nullptr);
        ui.startAcquisitionBtn->setText("Stop Acquisition");
    } else {
        spdlog::info("Stopping acquisition");
        m_acquisition->Abort();
        m_acquisition->WaitForStop();
        ui.startAcquisitionBtn->setText("Start Acquisition");
    }
}



/* void MainWindow::on_inputSpinBox1_valueChanged(int value) { */
/*     ui.outputWidget->setText(QString::number(value + ui.inputSpinBox2->value())); */
/* } */

/* void MainWindow::on_inputSpinBox2_valueChanged(int value) { */
/*     ui.outputWidget->setText(QString::number(value + ui.inputSpinBox1->value())); */
/* } */
