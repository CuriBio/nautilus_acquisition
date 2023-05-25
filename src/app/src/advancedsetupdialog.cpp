#include <format>
#include <spdlog/spdlog.h>

#include "advancedsetupdialog.h"
#include "ui_advancedsetupdialog.h"

/*
* Instance of advanced setup options window.
*
* @param parent Pointer to parent widget.
*/
AdvancedSetupDialog::AdvancedSetupDialog(std::shared_ptr<Config> config, QWidget *parent) : QDialog(parent), ui(new Ui::AdvancedSetupDialog) {
    ui->setupUi(this);
    m_config = config;

    connect(ui->updatesetupbtn, &QPushButton::released, this, &AdvancedSetupDialog::update_advanced_setup);
    connect(ui->nidevicelist, SIGNAL(currentIndexChanged(int)),this, SLOT(nidevice_indexChanged(int)));
}


AdvancedSetupDialog::~AdvancedSetupDialog() {
    delete ui;
}


/*
* Reads lists of ni devices and populates drop down menu,
* Initialize othere options.
*
* @param vector of ni device names
*/
void AdvancedSetupDialog::Initialize(std::vector<std::string> devicelist){
    ui->nidevicelist->clear();
    if(devicelist.size() == 0){
        ui->nidevicelist->addItem("No NI devices detected");
    }else{
        for(std::string nidevicename : devicelist){
            ui->nidevicelist->addItem(QString::fromStdString(nidevicename));
        }
    }
}


/*
* If options are confirmed then update them.
*/
void AdvancedSetupDialog::update_advanced_setup(){
    //only one setting to update right now

    //if new newdev selected then update toml and channels
    if(m_niDev != "No NI devices detected"){
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
* @param index of new choice
*/
void AdvancedSetupDialog::nidevice_indexChanged(int index){
    m_niDev = ui->nidevicelist->currentText().toStdString();
}


