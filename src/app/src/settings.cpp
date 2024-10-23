/*
 * MIT License
 *
 * Copyright (c) 2022 Curi Bio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <spdlog/spdlog.h>
#include <filesystem>

#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QPushButton>
#include <regex>

#include "settings.h"

/*
 * Constructs the settings dialog modal.
 *
 * @param parent The parent widget.
 * @param path The initial output path value.
 * @param prefix The initial file prefix value.
 */
Settings::Settings(QWidget* parent, std::shared_ptr<const Config> config) : QDialog(parent) {
    ui.setupUi(this);

    m_config = config;

    setupOptions();
}


/*
 * Settings destructor.
 */
Settings::~Settings() {
}


void Settings::setupOptions() {
    QString qpath = m_config->path.string().c_str();
    ui.dirChoice->setPlainText(qpath);
    ui.dirChoice->setPlaceholderText(qpath);

    ui.filePrefix->setText(m_config->prefix.c_str());
}

void Settings::show() {
    setupOptions();
    QDialog::show();
}

/*
 * Directory choice button slot, called when user clicks on directory selector.
 * Sets the output directory path.
 */
void Settings::on_dirChoiceBtn_clicked() {
    auto dir = QFileDialog::getExistingDirectory(this, "Select output directory", "E:\\");
    QString prefix = "E:";

    if (!dir.startsWith(prefix)) {
        if (dir.isEmpty()) {
            auto msg = "Cancelled changing output directory";
            spdlog::info(msg);
            spdlog::get("nautilai_gxp")->info(msg);
        } else {
            spdlog::error("Must use output directory on E:\\ drive, selected {}", dir.toStdString());
            spdlog::get("nautilai_gxp")->info("Set invalid output directory (not on E:\\ drive) {}", dir.toStdString());
            QMessageBox messageBox;
            messageBox.critical(0, "Error", "Must select output directory on E:\\ drive");
            messageBox.setFixedSize(500,200);
        }
    } else {
        auto msg = fmt::format("Selected new output directory: {}", dir.toStdString());
        spdlog::info(msg);
        spdlog::get("nautilai_gxp")->info(msg);
        ui.dirChoice->setPlainText(dir);
    }
}


void Settings::on_filePrefix_textChanged() {
    auto filePrefixStd = ui.filePrefix->text().toStdString();
    std::regex forbiddenCharRegex("[<>:\"/\\\\|?*]");
    bool isPrefixValid = filePrefixStd.length() <= 200 && !isspace(static_cast<unsigned char>(filePrefixStd[0])) && !std::regex_search(filePrefixStd, forbiddenCharRegex);
    QString newStyle = isPrefixValid ? "" : "border: 1px solid red";
    ui.filePrefix->setStyleSheet(newStyle);

    ui.modalChoice->button(QDialogButtonBox::Save)->setEnabled(isPrefixValid);
}

void Settings::on_filePrefix_editingFinished() {
    spdlog::get("nautilai_gxp")->info("Set file prefix '{}'", ui.filePrefix->text().toStdString());
}

/*
 * Emits signal for when a user accepts changes to settings.
 */
void Settings::on_modalChoice_accepted() {
    spdlog::get("nautilai_gxp")->info("New settings saved");
    emit sig_settings_changed(
        ui.dirChoice->toPlainText().toStdString(),
        ui.filePrefix->text().toStdString()
    );
    this->accept();
}


/*
 *
 * Cancels updating settings dialog value when user clicks cancel.
 */
void Settings::on_modalChoice_rejected() {
    spdlog::get("nautilai_gxp")->info("New settings discarded");
    this->reject();
}


void Settings::closeEvent(QCloseEvent *event) {
    spdlog::get("nautilai_gxp")->info("New settings discarded");
    this->reject();
}
