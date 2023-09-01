#include <format>
#include <spdlog/spdlog.h>

#include "advancedsetupdialog.h"
#include "ui_advancedsetupdialog.h"
#include <pm/Camera.h>

/*
* Instance of advanced setup options window.
*
* @param parent Pointer to parent widget.
*/
AdvancedSetupDialog::AdvancedSetupDialog(std::shared_ptr<Config> config, QWidget *parent) : QDialog(parent), ui(new Ui::AdvancedSetupDialog) {
    ui->setupUi(this);
    m_config = config;

    connect(ui->updatesetupbtn, &QPushButton::released, this, &AdvancedSetupDialog::update_advanced_setup);
}


AdvancedSetupDialog::~AdvancedSetupDialog() {
    delete ui;
}


/*
* Populate dropdowns.
*
* @param vector of ni device names
*/
void AdvancedSetupDialog::Initialize(std::vector<std::string> devicelist){
    ui->nidevicelist->clear();
    if (devicelist.size() == 0) {
        ui->nidevicelist->addItem("No NI devices detected");
    } else {
        for(std::string nidevicename : devicelist){
            ui->nidevicelist->addItem(QString::fromStdString(nidevicename));
        }
    }

    m_triggerMode = m_config->triggerMode;

    ui->triggerModeList->clear();
    ui->triggerModeList->addItem(QString("Wait for trigger"));
    ui->triggerModeList->addItem(QString("Start acquisition immediately"));

    int currentTrigModeIndex = -1;
    switch (m_triggerMode) {
        case EXT_TRIG_TRIG_FIRST:
            currentTrigModeIndex = 0;
        case EXT_TRIG_INTERNAL:
            currentTrigModeIndex = 1;
    }
    ui->triggerModeList->setCurrentIndex(currentTrigModeIndex);
}


/*
* Save the updates.
*/
void AdvancedSetupDialog::update_advanced_setup(){
    emit this->sig_trigger_mode_change(m_triggerMode);
    emit this->sig_enable_live_view_during_acquisition_change(m_enableLiveViewDuringAcquisition);

    //if new nidev selected then update toml and channels
    if (m_niDev != "No NI devices detected") {
        //save new ni device to toml file
        auto file = toml::parse(m_config->configFile);
        file["device"]["nidaqmx"]["device"] = m_niDev;

        std::ofstream outf(m_config->configFile);
        outf << std::setw(0) << file << std::endl;
        outf.close();

        emit this->sig_ni_dev_change(m_niDev);
    }
    this->close();
}


/*
* When user selects a different ni device, save changes to be confirmed later.
*
* @param text of new choice
*/
void AdvancedSetupDialog::on_nidevice_currentTextChanged(const QString &text) {
    m_niDev = text.toStdString();
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
* @param text of new choice
*/
void AdvancedSetupDialog::on_checkEnableLiveViewDuringAcq_stateChanged(int state) {
    m_enableLiveViewDuringAcquisition = state;
}
