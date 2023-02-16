#include <format>
#include <spdlog/spdlog.h>

#include <QFileDialog>

#include "advancedsetupdialog.h"
#include "ui_advancedsetupdialog.h"

AdvancedSetupDialog::AdvancedSetupDialog( QWidget *parent) : QDialog(parent), ui(new Ui::AdvancedSetupDialog) {
    ui->setupUi(this);
}

AdvancedSetupDialog::~AdvancedSetupDialog() {
    delete ui;
}