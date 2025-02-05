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
    m_displayRoisDuringLiveView = m_config->displayRoisDuringLiveView;

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
    ui->checkDisplayRoisDuringLiveView->setChecked(m_displayRoisDuringLiveView);

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
    spdlog::info("User confirmed advanced settings");

    emit this->sig_trigger_mode_change(m_triggerMode);
    emit this->sig_enable_live_view_during_acquisition_change(m_enableLiveViewDuringAcquisition);
    emit this->sig_display_rois_during_live_view_change(m_displayRoisDuringLiveView);

    m_config->enableDownsampleRawFiles = m_enableDownsampleRawFiles;
    m_config->binFactor = m_binFactor;
    m_config->keepOriginalRaw = m_keepOriginalRaw;

    if (m_enableDownsampleRawFiles) {
        spdlog::info("User enabled additional binning settings to a factor of {} and keep original to {}", m_binFactor, m_keepOriginalRaw);
    }

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
    if (text.toStdString() == (std::string) "Wait for trigger") {
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
* When user updates this checkbox, save changes to be confirmed later.
*
* @param new checked state
*/
void AdvancedSetupDialog::on_checkDisplayRoisDuringLiveView_stateChanged(int state) {
    m_displayRoisDuringLiveView = state;
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
    emit this->sig_close_adv_settings();
}
