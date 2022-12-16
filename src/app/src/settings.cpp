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
