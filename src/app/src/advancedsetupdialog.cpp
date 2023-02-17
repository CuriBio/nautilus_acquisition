#include <format>
#include <spdlog/spdlog.h>


#include "advancedsetupdialog.h"
#include "ui_advancedsetupdialog.h"

/*
* Instance of advanced setup options window.
*
* @param parent Pointer to parent widget.
*/
AdvancedSetupDialog::AdvancedSetupDialog( QWidget *parent) : QDialog(parent), ui(new Ui::AdvancedSetupDialog) {
    ui->setupUi(this);

    std::string new_niDev;

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
    if(!new_niDev.empty()){
        //TODO Update what ever is need to get updated for the ni device name
        spdlog::info("Updated NI device name in toml");
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
    spdlog::info("new");
}


