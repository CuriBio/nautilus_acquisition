#include <format>
#include <spdlog/spdlog.h>

#include "advancedsetupdialog.h"
#include "ui_advancedsetupdialog.h"

/*
* Instance of advanced setup options window.
*
* @param parent Pointer to parent widget.
*/
AdvancedSetupDialog::AdvancedSetupDialog( toml::value config,std::string* m_niDev,QWidget *parent) : QDialog(parent), ui(new Ui::AdvancedSetupDialog) {
    ui->setupUi(this);

    std::string niDev = toml::find_or<std::string>(config, "device", "nidaqmx","device", std::string("Dev2"));
    new_niDev = niDev;
    m_niDev=m_niDev;

    connect(ui->updatesetupbtn, &QPushButton::released, this, &AdvancedSetupDialog::on_confirm_new_advanced_setup);
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
void AdvancedSetupDialog::on_confirm_new_advanced_setup(){
    //only one setting to update right now

    //if new newdev selected then update toml and channels
    if(new_niDev != "No NI devices detected"){
        std::filesystem::path userProfile{"/Users"};
        char* up = getenv("USERPROFILE");
        if (up != nullptr) {
            userProfile = std::string(up);
        }
        //save new ni device to toml file
        std::filesystem::path configFile{fmt::format("{}/AppData/Local/Nautilus/nautilus.toml", userProfile.string())};
        auto file = toml::parse(configFile);
        file["device"]["nidaqmx"]["device"] = new_niDev;
        std::ofstream outf;
        outf.open(configFile.string());
        outf << file << std::endl;
        spdlog::info("Updated NI device name in toml");
        emit this->sig_ni_dev_change();
    }

    spdlog::info("Done updating advanced settings");
    this->close();
}


/*
* When user selects a different ni device, save changes to be confirmed later.
*
* @param index of new choice
*/
void AdvancedSetupDialog::nidevice_indexChanged(int index){
    new_niDev = ui->nidevicelist->currentText().toStdString();
}


std::string AdvancedSetupDialog::Get_Device_Selected(){
    return new_niDev;
}
