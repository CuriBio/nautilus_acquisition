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
 * @file  mainwindow.h
 *
 * @brief Definition of the mainwindow widget.
 *********************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <mutex>
#include <filesystem>
#include <string>

#include <toml.hpp>

#include <interfaces/CameraInterface.h>
#include <interfaces/AcquisitionInterface.h>
#include <interfaces/FrameInterface.h>

#include <NIDAQmx_wrapper.h>
#include <pm/Camera.h>
#include <pm/Frame.h>
#include <pm/Acquisition.h>
#include <pm/ColorConfig.h>
#include <pvcam/pvcam_helper_color.h>

#include <ParTask.h>
#include <TaskFrameStats.h>
#include <TaskFrameLut16.h>
#include <TaskApplyLut16.h>


#include "settings.h"
#include "ui_mainwindow.h"
#include "stagecontrol.h"
#include "advancedsetupdialog.h"

#define TASKS 8

using pmCamera = Camera<pm::Camera, pm::Frame>;
using pmAcquisition = Acquisition<pm::Acquisition, pm::ColorConfig, ph_color_context, pm::Camera, pm::Frame>;
using pmColorConfig = pm::ColorConfig<ph_color_context>;

/*
 * Nautilus main window class.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(
            std::string path,
            std::string prefix,
            std::string niDev,
            std::string testImgPath,
            double fps,
            double duration,
            double expTimeMs,
            uint16_t spdtable,
            double ledIntensity,
            uint32_t bufferCount,
            uint32_t frameCount,
            StorageType storageType,
            uint16_t triggerMode,
            uint16_t exposureMode,
            double maxVoltage,
            bool noAutoConBright,
            std::string stageComPort,
            std::string configFile,
            toml::value& config,
            QMainWindow* parent = nullptr
        );

        ~MainWindow() {
            delete m_lut16;
            m_lut16 = nullptr;

            delete m_hist;
            m_hist = nullptr;

            delete m_img8;
            m_img8 = nullptr;
        }

        void Initialize();

    signals:
        void sig_acquisition_done();
        void sig_livescan_stopped();

    public slots:
        void acquisition_done();
        void settings_changed(std::filesystem::path path, std::string prefix);

    private slots:
        void on_ledIntensityEdit_valueChanged(double value);
        void on_frameRateEdit_valueChanged(double value);
        void on_durationEdit_valueChanged(double value);

        void on_advancedSetupBtn_clicked();
        void on_liveScanBtn_clicked();
        void on_settingsBtn_clicked();
        void on_startAcquisitionBtn_clicked();

        void on_stageNavigationBtn_clicked() {
            m_stageControl->show();
        }

        void updateLiveView();

    private:
        Ui::MainWindow ui;
        StageControl* m_stageControl{nullptr};
        AdvancedSetupDialog * m_advancedSettingsDialog{nullptr};
        std::string m_stageComPort{};

        Settings* m_settings {nullptr};
        std::mutex m_lock;

        toml::value m_config{};
        std::string m_configFile{};

        std::shared_ptr<pmCamera> m_camera;
        std::unique_ptr<pmAcquisition> m_acquisition{nullptr};

        QThread* m_acqusitionThread {nullptr};
        QTimer* m_liveViewTimer {nullptr};

        double m_ledIntensity{50.0};
        double m_maxVoltage{1.4};
        double m_fps{0.0};
        double m_expTimeMS{0.0};
        double m_duration{0.0};
        uint16_t m_spdtable{0};

        NIDAQmx m_DAQmx; //NI-DAQmx controller for LEDs
        std::string m_niDev; //NI-DAQmx device name
        std::string m_taskAO, m_devAO;
        std::string m_taskDO, m_devDO;
        bool m_led{false};

        std::filesystem::path m_path;
        std::string m_prefix;
        std::string m_testImgPath;

        bool m_acquisitionRunning {false};
        bool m_liveScanRunning {false};
        bool m_autoConBright{true};

        uint8_t* m_img8;

        uint32_t m_width, m_height;
        uint32_t m_min, m_max;
        uint32_t m_hmax;

        uint8_t* m_lut16{nullptr};
        uint32_t* m_hist{nullptr};

        ParTask m_parTask{TASKS};
        std::shared_ptr<TaskFrameStats> m_taskFrameStats;
        std::shared_ptr<TaskFrameLut16> m_taskUpdateLut;
        std::shared_ptr<TaskApplyLut16> m_taskApplyLut;

        double m_curPosX{0}, m_curPosY{0};

        CameraInfo m_camInfo;
        ExpSettings m_expSettings {
            .acqMode = AcqMode::LiveCircBuffer,
            .region = {} ,
            .storageType = StorageType::Tiff,
            .spdTableIdx = 0,
            .expTimeMS = 0,
            .trigMode = EXT_TRIG_INTERNAL,
            .expModeOut = EXPOSE_OUT_GLOBAL_SHUTTER,
            .frameCount = 0,
            .bufferCount = 100
        };

    private:
        void StartAcquisition(bool saveToDisk);
        void StopAcquisition();

        void AutoConBright(const uint16_t* data);
        bool ledON(double voltage);
        bool ledOFF();
        bool ledSetVoltage(double voltage);
        bool available_space_in_default_drive( double fps,double duration);
        //acquire helper function
        void acquire(bool saveToDisk, std::string prefix);

        static void acquisitionThread(MainWindow* cls);
};

#endif