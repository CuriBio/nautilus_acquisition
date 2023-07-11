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
#include <QCloseEvent>

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

#define TASKS 4
#define TIMESTAMP_STR "%Y_%m_%d_%H%M%S"
#define RECORDING_DATE_FMT "%Y-%m-%d %H:%M:%S"

using pmCamera = Camera<pm::Camera, pm::Frame>;
using pmAcquisition = Acquisition<pm::Acquisition, pm::ColorConfig, ph_color_context, pm::Camera, pm::Frame>;
using pmColorConfig = pm::ColorConfig<ph_color_context>;

enum AppState {
    Uninitialized,
    Initializing,
    Idle,
    LiveViewBtnPress,
    LiveViewRunning,
    AcquisitionBtnPress,
    AcquisitionRunning,
    LiveViewAcquisitionRunning,
    AdvSetupBtnPress,
    AdvSetupOpen,
    AdvSetupClosed,
    SettingsBtnPress,
    SettingsOpen,
    SettingsClosed,
    UserCanceled,
    AcquisitionDone,
    PostProcessing,
    PostProcessingLiveView,
    PostProcessingDone,
    Error,
};

/*
 * Nautilus main window class.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(std::shared_ptr<Config> params, QMainWindow* parent = nullptr);
        ~MainWindow() { }
        void Initialize();

    signals:
        void sig_update_state(AppState newState);
        void sig_disable_all();
        void sig_enable_all();
        void sig_set_platmapFormat(QStringList qs);
        void sig_set_fps_duration(int maxfps, int fps, int duration);
        void sig_show_error(std::string msg);
        void sig_progress_start(std::string msg, int n);
        void sig_progress_update(size_t n);
        void sig_progress_text(std::string msg);
        void sig_progress_done();
        void sig_start_acquisition(bool saveToDisk);
        void sig_stop_acquisition();
        //void sig_acquisition_done();
        void sig_start_postprocess();
        void sig_start_analysis();
        void sig_start_encoding();

    public slots:

    private slots:
        void updateState(AppState newState);

        void on_liveScanBtn_clicked() { emit sig_update_state(LiveViewBtnPress); }
        void on_startAcquisitionBtn_clicked() { emit sig_update_state(AcquisitionBtnPress); }
        void on_advancedSetupBtn_clicked() { emit sig_update_state(AdvSetupBtnPress); }
        void on_settingsBtn_clicked() { emit sig_update_state(SettingsBtnPress); }

        void on_frameRateEdit_valueChanged(double value);
        void on_plateFormatDropDown_activated(int index);
        void on_durationEdit_valueChanged(double value);

        void on_stageNavigationBtn_clicked() {
            m_stageControl->show();
            ui.stageNavigationBtn->setEnabled(false);
        }

        void on_ledIntensityEdit_valueChanged(double value) {
            m_config->ledIntensity = value;
            double voltage = (m_config->ledIntensity / 100.0) * m_config->maxVoltage;
            ledSetVoltage(voltage);
        }

    private:
        Ui::MainWindow ui;
        std::shared_ptr<Config> m_config = nullptr;

        Settings* m_settings {nullptr};
        AdvancedSetupDialog* m_advancedSettingsDialog{nullptr};
        StageControl* m_stageControl{nullptr};

        std::shared_ptr<pmCamera> m_camera;
        std::unique_ptr<pmAcquisition> m_acquisition{nullptr};
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

        QThread* m_acquisitionThread {nullptr};
        QTimer* m_liveViewTimer {nullptr};
        QProgressDialog* m_acquisitionProgress {nullptr};

        QProcess m_extAnalysis;
        QProcess m_extVidEncoder;
        uint8_t m_extEncodingRetries{0};
        double m_extRetryBackoffms{250};

        std::vector<std::filesystem::path> m_plateFormats;
        int m_plateFormatCurrentIndex{-1}; 

        NIDAQmx m_DAQmx; //NI-DAQmx controller for LEDs
        std::string m_taskAO, m_devAO;
        std::string m_taskDO, m_devDO;
        bool m_led{false};

        std::future<void> m_niSetup = {};
        std::future<bool> m_stageCalibrate = {};

        uint32_t m_width, m_height;
        uint32_t m_min, m_max;
        uint32_t m_hmax;

        uint16_t* m_img16;
        uint8_t* m_lut16{nullptr};
        uint32_t* m_hist{nullptr};

        ParTask m_parTask{TASKS};
        std::shared_ptr<TaskFrameStats> m_taskFrameStats;
        std::shared_ptr<TaskFrameLut16> m_taskUpdateLut;
        std::shared_ptr<TaskApplyLut16> m_taskApplyLut;

        std::string m_testImgPath;
            
        char m_startAcquisitionTS[std::size(TIMESTAMP_STR)+4] = {};
        char m_recordingDateFmt[std::size(RECORDING_DATE_FMT)+4] = {};

        bool m_userCanceled{false};
        bool m_needsPostProcessing{false};

        std::mutex m_fileCleanupLock;
        std::condition_variable m_fileCleanupCond;

        std::mutex m_lock;
        std::mutex m_liveViewLock;
        AppState m_curState = Uninitialized;

        std::map<std::tuple<AppState, AppState>, std::function<void()>> m_appTransitions = {
            { {Uninitialized, Initializing}, [this]() {
                enableUI(false);
                m_curState = Initializing;
            }},
            { {Initializing, Idle}, [this]() { 
                enableUI(true);
                m_curState = Idle;
            }},
            //live view states
            { {Idle, LiveViewBtnPress}, [this]() {
                m_curState = (startLiveView()) ? LiveViewRunning : Error;
            }},
            { {LiveViewRunning, LiveViewBtnPress}, [this]() {
                m_curState = (stopLiveView()) ? Idle : Error;
            }},
            //acquisition states
            { {Idle, AcquisitionBtnPress}, [this]() {
                m_curState = (startAcquisition()) ? AcquisitionRunning : Error;
            }},
            { {AcquisitionRunning, AcquisitionBtnPress}, [this]() {
                m_curState = (stopAcquisition()) ? Idle : Error;
            }},
            { {AcquisitionRunning, AcquisitionDone}, [this]() {
                m_curState = (stopAcquisition()) ? Idle : Error;
            }},
            //live view + acquisition
            { {LiveViewRunning, AcquisitionBtnPress}, [this]() {
                m_curState = (startAcquisition_LiveViewRunning()) ? LiveViewAcquisitionRunning : Error;
            }},
            { {AcquisitionRunning, LiveViewBtnPress}, [this]() {
                m_curState = (startLiveView_AcquisitionRunning()) ? LiveViewAcquisitionRunning : Error;
            }},
            { {AcquisitionRunning, AcquisitionDone}, [this]() {
                m_curState = (stopAcquisition()) ? Idle : Error;
            }},
            { {AcquisitionRunning, PostProcessing}, [this]() {
                m_curState = (startPostProcessing()) ? PostProcessing : Error;
            }},
            { {LiveViewAcquisitionRunning, LiveViewBtnPress}, [this]() {
                m_curState = (stopLiveView_AcquisitionRunning()) ? AcquisitionRunning : Error;
            }},
            { {LiveViewAcquisitionRunning, AcquisitionBtnPress}, [this]() {
                m_curState = (stopAcquisition_LiveViewRunning()) ? LiveViewRunning : Error;
            }},
            { {LiveViewAcquisitionRunning, AcquisitionDone}, [this]() {
                m_curState = (stopAcquisition_LiveViewRunning()) ? LiveViewRunning : Error;
            }},
            { {LiveViewAcquisitionRunning, PostProcessing}, [this]() {
                m_curState = (startPostProcessing_LiveViewRunning()) ? PostProcessingLiveView : Error;
            }},
            { {PostProcessing, PostProcessingDone}, [this]() {
                m_curState = (postProcessingDone()) ? Idle : Error;
            }},
            { {PostProcessing, LiveViewBtnPress}, [this]() {
                m_curState = (startLiveView_PostProcessing()) ? PostProcessingLiveView : Error;
            }},
            { {PostProcessingLiveView, PostProcessingDone}, [this]() {
                m_curState = (postProcessingDone_LiveViewRunning()) ? LiveViewRunning : Error;
            }},
            { {PostProcessingLiveView, LiveViewBtnPress}, [this]() {
                m_curState = (stopLiveView_PostProcessing()) ? PostProcessing : Error;
            }},
            //advanced setup
            { {Idle, AdvSetupBtnPress}, [this]() {
                m_curState = (advSetupOpen()) ? AdvSetupOpen : Error; 
            }},
            { {AdvSetupOpen, AdvSetupClosed}, [this]() {
                m_curState = (advSetupClosed()) ? Idle : Error;  
            }},
            //settings dialog
            { {Idle, SettingsBtnPress}, [this]() {
                m_curState = (settingsOpen()) ? SettingsOpen : Error;
            }},
            { {SettingsOpen, SettingsClosed}, [this]() {
                m_curState = (settingsClosed()) ? Idle : Error;
            }},
        };
        
    private:
        void closeEvent(QCloseEvent *event);

        bool startLiveView();
        bool startLiveView_PostProcessing();
        bool stopLiveView();
        bool startLiveView_AcquisitionRunning();
        bool stopLiveView_AcquisitionRunning();
        bool stopLiveView_PostProcessing();
        bool startAcquisition();
        bool stopAcquisition();
        bool startAcquisition_LiveViewRunning();
        bool stopAcquisition_LiveViewRunning();
        bool advSetupOpen();
        bool advSetupClosed();
        bool settingsOpen();
        bool settingsClosed();
        bool startPostProcessing();
        bool startPostProcessing_LiveViewRunning();
        bool postProcessingDone();
        bool postProcessingDone_LiveViewRunning();

        void enableUI(bool enable);

        void setupNIDev(std::string niDev);

        bool ledON(double voltage, bool delay=true);
        bool ledOFF();
        bool ledSetVoltage(double voltage);

        void settingsChanged(std::filesystem::path path, std::string prefix);

        bool availableDriveSpace(double fps, double duration, size_t nStagePositions);
        std::vector<std::filesystem::path> getFileNamesFromDirectory(std::filesystem::path path);
        QStringList vectorToQStringList(const std::vector<std::filesystem::path>& paths);

        void updateLiveView() noexcept;
        void autoConBright(const uint16_t* data);

        void acquisitionDone(bool runPostProcess); 
        static void acquisitionThread(MainWindow* cls);
        void postAcquisition();
        void postProcess();
};
#endif
