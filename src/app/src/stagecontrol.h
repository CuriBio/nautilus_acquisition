#ifndef STAGECONTROL_H
#define STAGECONTROL_H

#define STAGE_MOVE1 5
#define STAGE_MOVE2 15
#define STAGE_MOVE3 25

#include <vector>
#include <future>

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <toml.hpp>
#include <spdlog/spdlog.h>

#include "config.h"
#include <TangoStage.h>

enum InputMasks {
    AddPos = (1 << 0),
    DeletePos = (1 << 1),
    GotoPos = (1 << 2),
    SaveList = (1 << 3),
    LoadList = (1 << 4),
    MoveControls = (1 << 5),
};

// #define ENABLE_ALL = AddPos | DeletePos | GotoPos | SaveList | LoadList | MoveControls
// #define DISABLE_ALL = 0

namespace Ui {
class StageControl;
}

class StagePosition : public QListWidgetItem  {
    public:
        // suffixing with underscore since "pos" seems to have issues, possibly a member of the parent class
        int pos_;
        double x;
        double y;
        bool skipped;

    public:
        StagePosition(int pos, double xPos, double yPos) : QListWidgetItem(fmt::format("pos_{} - x: {}, y: {}", pos, xPos, yPos).c_str()) {
            pos_ = pos;
            x = xPos;
            y = yPos;
            skipped = false;
        }

        ~StagePosition() {}
};

class StageControl : public QDialog {
    Q_OBJECT

    private:
        Ui::StageControl *ui;
        std::vector<StagePosition*> m_positions;

        TangoStage* m_tango;
        std::string m_comPort;
        std::string m_configFile;
        std::shared_ptr<Config> m_config;

        std::vector<int> m_stepSizes;
        double m_curX{0.0}, m_curY{0.0};

        uint32_t m_inputMask, m_prevInputMask;

    public:
        explicit StageControl(std::string comPort, std::shared_ptr<Config> config, std::vector<int> stepSizes, QWidget *parent = nullptr);
        ~StageControl();

        bool Connected() const;
        bool Calibrate();
        void SetRelativePosition(double x, double y);
        void SetAbsolutePosition(double x, double y);

        void SetRelativeX(double x);
        void SetAbsoluteX(double x);

        void SetRelativeY(double y);
        void SetAbsoluteY(double y);

        const std::vector<StagePosition*>& GetPositions() const;

        void loadList(std::string fileName);

        // TODO log when this is closed

    signals:
        void sig_stagelist_updated();
        void sig_stage_disable_all();
        void sig_stage_enable_all();
        void sig_start_move();
        void sig_end_move();

    private slots:
        void on_skipBtn_clicked();
        void on_unskipBtn_clicked();

        void on_gotoPosBtn_clicked();

        void on_stageRightBtn1_clicked() {
            spdlog::get("nautilai_gxp")->info("Stage Right 1 pressed");
            SetRelativeX(m_stepSizes[0]);
        };
        void on_stageRightBtn2_clicked() {
            spdlog::get("nautilai_gxp")->info("Stage Right 2 pressed");
            SetRelativeX(m_stepSizes[1]);
        };
        void on_stageRightBtn3_clicked() {
            spdlog::get("nautilai_gxp")->info("Stage Right 3 pressed");
            SetRelativeX(m_stepSizes[2]);
        };

        void on_stageLeftBtn1_clicked() {
            spdlog::get("nautilai_gxp")->info("Stage Left 1 pressed");
            SetRelativeX(-m_stepSizes[0]);
        };
        void on_stageLeftBtn2_clicked() {
            spdlog::get("nautilai_gxp")->info("Stage Left 2 pressed");
            SetRelativeX(-m_stepSizes[1]);
        };
        void on_stageLeftBtn3_clicked() {
            spdlog::get("nautilai_gxp")->info("Stage Left 3 pressed");
            SetRelativeX(-m_stepSizes[2]);
        };

        void on_stageUpBtn1_clicked() {
            spdlog::get("nautilai_gxp")->info("Stage Up 1 pressed");
            SetRelativeY(m_stepSizes[0]);
        };
        void on_stageUpBtn2_clicked() {
            spdlog::get("nautilai_gxp")->info("Stage Up 2 pressed");
            SetRelativeY(m_stepSizes[1]);
        };
        void on_stageUpBtn3_clicked() {
            spdlog::get("nautilai_gxp")->info("Stage Up 3 pressed");
            SetRelativeY(m_stepSizes[2]);
        };

        void on_stageDownBtn1_clicked() {
            spdlog::get("nautilai_gxp")->info("Stage Down 1 pressed");
            SetRelativeY(-m_stepSizes[0]);
        };
        void on_stageDownBtn2_clicked() {
            spdlog::get("nautilai_gxp")->info("Stage Down 3 pressed");
            SetRelativeY(-m_stepSizes[1]);
        };
        void on_stageDownBtn3_clicked() {
            spdlog::get("nautilai_gxp")->info("Stage Down 2 pressed");
            SetRelativeY(-m_stepSizes[2]);
        };


    private:
        void closeEvent(QCloseEvent *event);

        void disableAll();
        void enableAll();
};

#endif // STAGECONTROL_H
