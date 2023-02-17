#include <format>
#include <spdlog/spdlog.h>

#include "advancedsetupdialog.h"
#include "ui_advancedsetupdialog.h"

/*
* Instance of advanced setup options window.
*
* @param parent Pointer to parent widget.
*/
AdvancedSetupDialog::AdvancedSetupDialog(m ,QWidget *parent) : QDialog(parent), ui(new Ui::AdvancedSetupDialog) {
    ui->setupUi(this);


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
    if(!new_niDev.empty()){
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

        //Re setup NIDAQmx controller for LED
        new_taskAO = "Analog_Out_Volts"; //Task for setting Analog Output voltage
        new_devAO = fmt::format("{}/ao0", new_niDev); //Device name for analog output
        spdlog::info("Using NI device {} for analog output", new_devAO);
        new_taskDO = "Digital_Out"; //Task for setting Digital Output
        new_devDO = fmt::format("{}/port0/line0:7", new_niDev); //Device for digital output
        spdlog::info("Using NI device {} for digital output", new_devDO);
        new_DAQmx.CreateTask(m_taskAO);
        new_DAQmx.CreateTask(m_taskDO);
        new_DAQmx.CreateAnalogOutpuVoltageChan(new_taskAO, new_devAO.c_str(), -10.0, 10.0, DAQmx_Val_Volts);
        new_DAQmx.CreateDigitalOutputChan(new_taskDO, new_devDO.c_str(), DAQmx_Val_ChanForAllLines);
        spdlog::info("Initialized NI device channels with new name");
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


