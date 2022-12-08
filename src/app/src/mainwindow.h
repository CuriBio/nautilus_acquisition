#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <mutex>
#include <filesystem>
#include <string>

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

#define TASKS 8

using pmCamera = Camera<pm::Camera, pm::Frame>;
using pmAcquisition = Acquisition<pm::Acquisition, pm::ColorConfig, ph_color_context, pm::Camera, pm::Frame>;
using pmColorConfig = pm::ColorConfig<ph_color_context>;

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(
            std::string path,
            std::string prefix,
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

        void updateLiveView();

    private:
        Ui::MainWindow ui;
        Settings* m_settings {nullptr};
        std::mutex m_lock;

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
        std::string m_taskAO, m_devAO;
        std::string m_taskDO, m_devDO;
        bool m_led{false};

        std::filesystem::path m_path;
        std::string m_prefix;

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


    private:
        void StartAcquisition(bool saveToDisk);
        void StopAcquisition();

        void AutoConBright(const uint16_t* data);
        bool ledON(double voltage);
        bool ledOFF();
        bool ledSetVoltage(double voltage);

        static void liveViewThreadFn(MainWindow* cls);
        static void acquisitionThread(MainWindow* cls, bool saveToDisk);
};

#endif

