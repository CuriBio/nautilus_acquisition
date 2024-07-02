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

/*********************************************************************
 * @file  autoupdate.h
 *
 * @brief Defines class for performing autoupdates
 *********************************************************************/
#ifndef AUTOUPDATE_H
#define AUTOUPDATE_H
#include <format>
#include <stdlib.h>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <QDialog>
#include <QProcess>
#include <QWidget>

#include "config.h"
#include "ui_autoupdate.h"

namespace Ui {
    class AutoUpdate;
}

class AutoUpdate : public QDialog {
    Q_OBJECT

    private:
        std::string m_origin; //= "https://downloads.curibio.com/software/nautilai";
        std::string m_url{};
        std::string m_channel;
        std::string m_file;
        std::filesystem::path m_updatePath{};
        std::shared_ptr<Config> m_config;

        Ui::AutoUpdate *ui;

    public:
        explicit AutoUpdate(std::shared_ptr<Config> config, std::string origin, std::string channel, QWidget *parent = nullptr);
        ~AutoUpdate() { delete ui; }

        bool hasUpdate();
        void applyUpdate();
        void show();

    private:
        bool downloadManifest();

    signals:
        void sig_start_update();

    private slots:
        void on_ignoreUpdate_clicked() {
            if (m_config && m_config->updateAvailable) {
                //TODO delete update files
                spdlog::info("Update ignored, removing installer");
                std::filesystem::remove_all(m_updatePath);
            }
            QDialog::close();
        }
        void on_acceptUpdate_clicked() {
            spdlog::info("Update accepted");

            if (m_config && m_config->updateAvailable) {
                emit sig_start_update();
            }
            QDialog::close();
        }
};
#endif //AUTOUPDATE_H
