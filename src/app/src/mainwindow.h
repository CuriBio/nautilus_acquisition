#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <mutex>

#include <interfaces/CameraInterface.h>
#include <interfaces/AcquisitionInterface.h>
#include <interfaces/FrameInterface.h>

#include <pm/Camera.h>
#include <pm/Frame.h>
#include <pm/Acquisition.h>
#include <pm/ColorConfig.h>
#include <pvcam/pvcam_helper_color.h>

#include "ui_mainwindow.h"

using pmCamera = Camera<pm::Camera, pm::Frame>;
using pmAcquisition = Acquisition<pm::Acquisition, pm::ColorConfig, ph_color_context, pm::Camera, pm::Frame>;
using pmColorConfig = pm::ColorConfig<ph_color_context>;

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QMainWindow *parent = nullptr);
        void Initialize();

    signals:
        void sig_acquisition_done();
        void sig_livescan_stopped();

    public slots:
        void acquisition_done();
        void livescan_stopped();

    private slots:
        void on_ledIntensityEdit_valueChanged(int value);
        void on_frameRateEdit_valueChanged(int value);
        void on_durationEdit_valueChanged(int value);

        void on_advancedSetupBtn_clicked();
        void on_liveScanBtn_clicked();
        void on_settingsBtn_clicked();
        void on_startAcquisitionBtn_clicked();

    private:
        Ui::MainWindow ui;
        std::mutex m_lock;

        std::shared_ptr<pmCamera> m_camera;
        std::unique_ptr<pmAcquisition> m_acquisition{nullptr};

        QThread* m_acqusitionThread {nullptr};
        QThread* m_liveViewThread {nullptr};
        bool m_stopLiveView {false};

        int m_duration{0};
        float m_fps{0.0};

        CameraInfo m_camInfo;
        ExpSettings m_expSettings {
            .acqMode = AcqMode::LiveCircBuffer,
            .fileName = "default_stack",
            .region = {} ,
            .imgFormat = ImageFormat::Mono16,
            .spdTableIdx = 0,
            .expTimeMS = 0,
            .trigMode = EXT_TRIG_INTERNAL,
            .expModeOut = EXPOSE_OUT_GLOBAL_SHUTTER,
            .frameCount = 0,
            .bufferCount = 50 //TODO allow user setting
        };

    private:
        static void liveViewThreadFn(MainWindow* cls);
        static void acquisitionThread(MainWindow* cls, bool saveToDisk);
};

#endif

