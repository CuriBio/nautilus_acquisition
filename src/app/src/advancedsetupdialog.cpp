#include <format>
#include <spdlog/spdlog.h>

#include "advancedsetupdialog.h"
#include "ui_advancedsetupdialog.h"
#include <pm/Camera.h>
#include <string>
#include <QCloseEvent>

/*
* Instance of advanced setup options window.
*
* @param parent Pointer to parent widget.
*/
AdvancedSetupDialog::AdvancedSetupDialog(std::shared_ptr<Config> config, QWidget *parent) : QDialog(parent), ui(new Ui::AdvancedSetupDialog) {
    ui->setupUi(this);
    m_config = config;
    changesConfirmed = false;
    connect(ui->updateSetupBtn, &QPushButton::released, this, &AdvancedSetupDialog::updateAdvancedSetup);
    setDefaultValues();
}


AdvancedSetupDialog::~AdvancedSetupDialog() {
    delete ui;
}


void AdvancedSetupDialog::show() {
    setDefaultValues();
    QDialog::show();
}


/*
* Populate dropdowns.
*
* @param vector of ni device names
*/
void AdvancedSetupDialog::Initialize(std::vector<std::string> devicelist){
    ui->ledDeviceList->clear();
    if (devicelist.size() == 0) {
        ui->ledDeviceList->addItem("No NI devices detected");
        ui->triggerDeviceList->addItem("No NI devices detected");
    } else {
        for (int i=0; i<devicelist.size(); i++) {
            ui->ledDeviceList->addItem(QString::fromStdString(devicelist[i]));
            ui->triggerDeviceList->addItem(QString::fromStdString(devicelist[i]));
        }
    }

    for(const auto& key_value : m_config->videoQualityOptions) {
        ui->videoQualityList->addItem(QString::fromStdString(key_value.first));
    }

    ui->triggerModeList->clear();
    ui->triggerModeList->addItem(QString("Wait for trigger"));
    ui->triggerModeList->addItem(QString("Start acquisition immediately"));
}

void AdvancedSetupDialog::setDefaultValues() {
    m_triggerMode = m_config->triggerMode;
    m_binFactor = m_config->binFactor;
    m_enableDownsampleRawFiles = m_config->enableDownsampleRawFiles;
    m_keepOriginalRaw = m_config->keepOriginalRaw;
    m_enableLiveViewDuringAcquisition = m_config->enableLiveViewDuringAcquisition;

    for (int i=0; i<ui->ledDeviceList->count(); i++) {
        if (ui->ledDeviceList->itemText(i).toStdString() == m_config->niDev) {
            ui->ledDeviceList->setCurrentIndex(i);
        }
    }
    for (int i=0; i<ui->triggerDeviceList->count(); i++) {
        if (ui->triggerDeviceList->itemText(i).toStdString() == m_config->trigDev) {
            ui->triggerDeviceList->setCurrentIndex(i);
        }
    }
    for (int i=0; i<ui->videoQualityList->count(); i++) {
        if (ui->videoQualityList->itemText(i).toStdString() == m_config->selectedVideoQualityOption) {
            ui->videoQualityList->setCurrentIndex(i);
        }
    }

    int currentTrigModeIndex = -1;
    switch (m_triggerMode) {
        case EXT_TRIG_TRIG_FIRST:
            currentTrigModeIndex = 0;
            break;
        case EXT_TRIG_INTERNAL:
            currentTrigModeIndex = 1;
            break;
    }
    // update to most recent user-confirmed state
    ui->triggerModeList->setCurrentIndex(currentTrigModeIndex);

    ui->checkEnableLiveViewDuringAcq->setChecked(m_enableLiveViewDuringAcquisition);

    ui->binFactorList->setCurrentIndex((m_binFactor / 2) - 1);
    ui->binFactorList->setEnabled(m_enableDownsampleRawFiles);

    ui->checkDownsampleRawFiles->setChecked(m_enableDownsampleRawFiles);

    ui->checkKeepOriginalRaw->setChecked(m_keepOriginalRaw);
    ui->checkKeepOriginalRaw->setEnabled(m_enableDownsampleRawFiles);
}

/*
* Save the updates.
*/
void AdvancedSetupDialog::updateAdvancedSetup(){
    changesConfirmed = true;

    emit this->sig_trigger_mode_change(m_triggerMode);
    emit this->sig_enable_live_view_during_acquisition_change(m_enableLiveViewDuringAcquisition);

    m_config->enableDownsampleRawFiles = m_enableDownsampleRawFiles;
    m_config->binFactor = m_binFactor;
    m_config->keepOriginalRaw = m_keepOriginalRaw;

    //if new nidev selected then update toml and channels
    if (m_niDev != "No NI devices detected") {
        // save new ni device to toml file
        auto file = toml::parse(m_config->configFile);
        file["device"]["nidaqmx"]["device"] = m_niDev;
        file["device"]["nidaqmx"]["device_2"] = m_trigDev;

        std::ofstream outf(m_config->configFile);
        outf << std::setw(0) << file << std::endl;
        outf.close();

        emit this->sig_ni_dev_change(m_niDev, m_trigDev);
    }

    m_config->selectedVideoQualityOption = ui->videoQualityList->currentText().toStdString();

    auto msg = fmt::format(
        "New advanced settings saved: "
        "led-ni=device='{}', trigger-ni-device='{}', trigger-in-mode='{}', enable-live-view-during-acquisition='{}', "
        "downsample-raw-data='{}', binning-factor='{}' keep-original-raw-data-after-downsample='{}', "
        "video-quality='{}' ",
        m_niDev, m_trigDev, m_triggerMode, m_enableLiveViewDuringAcquisition,
        m_enableDownsampleRawFiles, m_binFactor, m_keepOriginalRaw,
        m_config->selectedVideoQualityOption,
    );
    spdlog::info(msg);
    spdlog::get("nautilai_gxp")->info(msg);

    this->close();
}


/*
* When user selects a different ni device, save changes to be confirmed later.
*
* @param text of new choice
*/
void AdvancedSetupDialog::on_ledDeviceList_currentTextChanged(const QString &text) {
    m_niDev = text.toStdString();
}

/*
* When user selects a different trigger device, save changes to be confirmed later.
*
* @param text of new choice
*/
void AdvancedSetupDialog::on_triggerDeviceList_currentTextChanged(const QString &text) {
    m_trigDev = text.toStdString();
}


/*
* When user selects a different trigger mode, save changes to be confirmed later.
*
* @param text of new choice
*/
void AdvancedSetupDialog::on_triggerModeList_currentTextChanged(const QString &text) {
    auto textStd = text.toStdString();
    if (textStd == "Wait for trigger") {
        m_triggerMode = EXT_TRIG_TRIG_FIRST;
    } else {  // "Start acquisition immediately"
        m_triggerMode = EXT_TRIG_INTERNAL;
    }
}


/*
* When user updates this checkbox, save changes to be confirmed later.
*
* @param new checked state
*/
void AdvancedSetupDialog::on_checkEnableLiveViewDuringAcq_stateChanged(int state) {
    m_enableLiveViewDuringAcquisition = state;
}

/*
* When user updates this checkbox, disabled/enable binning factor dropdown accordingly.
*
* @param new checked state
*/
void AdvancedSetupDialog::on_checkDownsampleRawFiles_stateChanged(int state) {
    // enable additional downsample settings
    ui->binFactorList->setEnabled(state);
    ui->checkKeepOriginalRaw->setEnabled(state);

    m_enableDownsampleRawFiles = state;

    // reset additional settings
    if (!state) {
        ui->binFactorList->setCurrentIndex(0);
        ui->checkKeepOriginalRaw->setChecked(false);
    }
}

/*
* When user updates binning factor, save selection as int to be confirmed later.
*
* @param text of new choice
*/
void AdvancedSetupDialog::on_binFactorList_currentTextChanged(const QString &text) {
    m_binFactor = text.toInt();
}

/*
* When user updates this checkbox, save changes for confirmation.
*
* @param new checked state
*/
void AdvancedSetupDialog::on_checkKeepOriginalRaw_stateChanged(int state) {
    m_keepOriginalRaw = state;
}

void AdvancedSetupDialog::closeEvent(QCloseEvent *event) {
    if (!changesConfirmed) {
        spdlog::get("nautilai_gxp")->info("New advanced settings discarded");
    }
    emit this->sig_close_adv_settings();
}
