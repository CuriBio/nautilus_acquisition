#include <spdlog/spdlog.h>
#include <filesystem>

#include <QFileDialog>
#include <QString>
#include "settings.h"

Settings::Settings(QWidget* parent, std::filesystem::path path, std::string prefix) : QDialog(parent) {
    ui.setupUi(this);

    QString qpath = path.string().c_str();
    ui.dirChoice->setPlainText(qpath);
    ui.dirChoice->setPlaceholderText(qpath);

    ui.filePrefix->setPlainText(prefix.c_str());
}

Settings::~Settings() {
}

void Settings::on_dirChoiceBtn_clicked() {
    spdlog::info("Dir Choice button clicked");
    auto dir = QFileDialog::getExistingDirectory(this, "Select output directory", "C:\\Users");

    ui.dirChoice->setPlainText(dir);
    spdlog::info("Selected dir: {}", dir.toStdString());
}

void Settings::on_modalChoice_accepted() {
    spdlog::info("Accepted");
    emit sig_settings_changed(
        ui.dirChoice->toPlainText().toStdString(),
        ui.filePrefix->toPlainText().toStdString()
    );
    this->accept();
}

void Settings::on_modalChoice_rejected() {
    spdlog::info("Rejected");
    this->reject();
}
