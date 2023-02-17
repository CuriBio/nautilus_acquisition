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
    m_niDev = nullptr;

    connect(ui->updatesetupbtn, &QPushButton::released, this, &AdvancedSetupDialog::on_confirm_new_advanced_setup);
    connect(ui->nidevicelist, SIGNAL(currentIndexChanged(int)),this, SLOT(AdvancedSetupDialog::indexChanged(int)));
}


AdvancedSetupDialog::~AdvancedSetupDialog() {
    delete ui;
}


/*
* Reads lists of ni devices and populates drop down menu,Initialize othere options.
*
* @param vector of ni devices
* @param pointer to current m_niDev from mainwindow
*/
void AdvancedSetupDialog::Initialize(std::vector<char> devicelist,std::string* m_niDev){
    ui->nidevicelist->clear();
    if(devicelist.size() == 0){
        ui->nidevicelist->addItem("No NI devices detected");
    }else{
        this->m_niDev = m_niDev;
    }


}


/*
* If options are confirmed then update them.
*/
void AdvancedSetupDialog::on_confirm_new_advanced_setup(){
    //only one setting to update right now
    if(!new_niDev.empty()){
        *m_niDev = new_niDev;
        spdlog::info("Updated NI device name");
    }
    spdlog::info("Done updating advanced settings");
    this->close();
}


/*
* When user selects a different ni device, save changes to be confirmed later.
*
* @param index of new choice
*/
void AdvancedSetupDialog::indexChanged(int index){
    new_niDev = ui->nidevicelist->currentText().toStdString();
}


