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
#include <vector>

#include <toml.hpp>
#include <tsl/ordered_map.h>
#include <QProgressDialog>
#include <QComboBox>
#include <QStringList>
#include <QProcess>
#include <QProgressDialog>

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


#include "config.h"
#include "settings.h"
#include "ui_mainwindow.h"
#include "stagecontrol.h"
#include "advancedsetupdialog.h"

#define TASKS 8
#define TIMESTAMP_STR "%Y_%m_%d_%H%M%S"

using pmCamera = Camera<pm::Camera, pm::Frame>;
using pmAcquisition = Acquisition<pm::Acquisition, pm::ColorConfig, ph_color_context, pm::Camera, pm::Frame>;
using pmColorConfig = pm::ColorConfig<ph_color_context>;

/*
 * Nautilus main window class.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(std::shared_ptr<Config> params, QMainWindow* parent = nullptr);

        ~MainWindow() {
            delete m_lut16;
            m_lut16 = nullptr;

            delete m_hist;
            m_hist = nullptr;

            delete m_img16;
            m_img16 = nullptr;

            delete m_acquisitionProgress;
            m_acquisitionProgress = nullptr;
        }

        void Initialize();

    signals:
        void sig_progress_start(std::string msg, int n);
        void sig_progress_update(size_t n);
        void sig_progress_text(std::string msg);
        void sig_progress_done();
        void sig_acquisition_done(bool runPostProcess);
        void sig_livescan_stopped();
        void sig_start_analysis();
        void sig_start_encoding();
        void sig_enable_controls(bool enable);
        void sig_show_error(std::string msg);
        void sig_set_fps_duration(int maxfps, int fps, int duration);
        void sig_set_platmapFormat(QStringList qs);

    public slots:
        void acquisition_done(bool runPostProcess);
        void settings_changed(std::filesystem::path path, std::string prefix);
        void stagelist_updated(size_t count);

    private slots:
        void on_ledIntensityEdit_valueChanged(double value);
        void on_frameRateEdit_valueChanged(double value);
        void on_durationEdit_valueChanged(double value);
        void on_plateFormatDropDown_activated(int index);


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

        std::shared_ptr<Config> m_config = nullptr;
        Settings* m_settings {nullptr};
        std::mutex m_lock;

        std::string m_configFile{};

        std::shared_ptr<pmCamera> m_camera;
        std::unique_ptr<pmAcquisition> m_acquisition{nullptr};

        QThread* m_acqusitionThread {nullptr};
        QTimer* m_liveViewTimer {nullptr};
        QProgressDialog* m_acquisitionProgress {nullptr};
        QProcess m_extAnalysis;
        QProcess m_extVidEncoder;

        NIDAQmx m_DAQmx; //NI-DAQmx controller for LEDs
        std::string m_taskAO, m_devAO;
        std::string m_taskDO, m_devDO;
        bool m_led{false};

        std::filesystem::path m_path;
        std::string m_prefix;
        std::string m_testImgPath;

        char m_startAcquisitionTS[std::size(TIMESTAMP_STR)+4] = {};

        bool m_acquisitionRunning {false};
        bool m_liveScanRunning {false};
        bool m_userCanceled {false};

        std::vector<std::filesystem::path> m_plateFormats;

        uint16_t* m_img16;

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

        std::future<void> m_niSetup = {};
        std::future<bool> m_stageCalibrate = {};

        QLabel* m_waitingLabel;
        QMovie* m_waitingMov;

    private:
        void EnableAll(bool enable);

        void StartAcquisition(bool saveToDisk);
        void StopAcquisition();

        bool availableDriveSpace(double fps, double duration, size_t nStagePositions);
        void AutoConBright(const uint16_t* data);
        bool ledON(double voltage);
        bool ledOFF();
        bool ledSetVoltage(double voltage);
        void setupNIDev(std::string new_m_niDev);
        double calcMaxFrameRate(uint16_t p1, uint16_t p2, double line_time);

        //postProcess helper
        void postProcess();

        //acquire helper function
        void acquire(bool saveToDisk);

        std::vector<std::filesystem::path> getFileNamesFromDirectory(std::filesystem::path path);
        QStringList vectorToQStringList(const std::vector<std::filesystem::path>& paths);

        //threads
        static void acquisitionThread(MainWindow* cls);
};
#endif
