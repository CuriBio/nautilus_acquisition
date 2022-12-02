#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <mutex>
#include <filesystem>
#include <string>

#include <interfaces/CameraInterface.h>
#include <interfaces/AcquisitionInterface.h>
#include <interfaces/FrameInterface.h>

#include <pm/Camera.h>
#include <pm/Frame.h>
#include <pm/Acquisition.h>
#include <pm/ColorConfig.h>
#include <pvcam/pvcam_helper_color.h>

#include "settings.h"
#include "ui_mainwindow.h"

using pmCamera = Camera<pm::Camera, pm::Frame>;
using pmAcquisition = Acquisition<pm::Acquisition, pm::ColorConfig, ph_color_context, pm::Camera, pm::Frame>;
using pmColorConfig = pm::ColorConfig<ph_color_context>;

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(
            std::string path,
            std::string prefix,
            uint32_t fps,
            double duration,
            uint16_t spdtable,
            double ledIntensity,
            uint32_t bufferCount,
            uint32_t frameCount,
            StorageType storageType,
            uint16_t triggerMode,
            uint16_t exposureMode,
            QMainWindow* parent = nullptr
        );
        void Initialize();

    signals:
        void sig_acquisition_done();
        void sig_livescan_stopped();

    public slots:
        void acquisition_done();
        void settings_changed(std::filesystem::path path, std::string prefix);

    private slots:
        void on_ledIntensityEdit_valueChanged(int value);
        void on_frameRateEdit_valueChanged(int value);
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

        double m_ledIntensity{0.0};
        uint32_t m_fps{0};
        double m_duration{0};
        uint16_t m_spdtable{0};

        std::filesystem::path m_path;
        std::string m_prefix;

        CameraInfo m_camInfo;
        ExpSettings m_expSettings {
            .acqMode = AcqMode::LiveCircBuffer,
            .region = {} ,
            .imgFormat = ImageFormat::Mono16,
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

    private:
        void StartAcquisition(bool saveToDisk);
        void StopAcquisition();
        static void liveViewThreadFn(MainWindow* cls);
        static void acquisitionThread(MainWindow* cls, bool saveToDisk);
};

#endif

