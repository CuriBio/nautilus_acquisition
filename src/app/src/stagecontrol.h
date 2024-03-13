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
        int pos;
        double x;
        double y;
        bool skipped;

    public:
        StagePosition(int pos, double xPos, double yPos) : QListWidgetItem(fmt::format("pos_{} - x: {}, y: {}", pos, xPos, yPos).c_str()) {
            pos = pos;
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

        void saveList(std::string fileName, bool fileExists);
        void loadList(std::string fileName);

    signals:
        void sig_stagelist_updated(size_t count);
        void sig_stage_disable_all();
        void sig_stage_enable_all();
        void sig_start_move();
        void sig_end_move();

    private slots:
        void on_skipBtn_clicked();
        void on_unskipBtn_clicked();

        void on_gotoPosBtn_clicked();

        void on_stageRightBtn1_clicked() { SetRelativeX(m_stepSizes[0]); };
        void on_stageRightBtn2_clicked() { SetRelativeX(m_stepSizes[1]); };
        void on_stageRightBtn3_clicked() { SetRelativeX(m_stepSizes[2]); };

        void on_stageLeftBtn1_clicked() { SetRelativeX(-m_stepSizes[0]); };
        void on_stageLeftBtn2_clicked() { SetRelativeX(-m_stepSizes[1]); };
        void on_stageLeftBtn3_clicked() { SetRelativeX(-m_stepSizes[2]); };

        void on_stageUpBtn1_clicked() { SetRelativeY(m_stepSizes[0]); };
        void on_stageUpBtn2_clicked() { SetRelativeY(m_stepSizes[1]); };
        void on_stageUpBtn3_clicked() { SetRelativeY(m_stepSizes[2]); };

        void on_stageDownBtn1_clicked() { SetRelativeY(-m_stepSizes[0]); };
        void on_stageDownBtn2_clicked() { SetRelativeY(-m_stepSizes[1]); };
        void on_stageDownBtn3_clicked() { SetRelativeY(-m_stepSizes[2]); };


    private:
        void disableAll();
        void enableAll();
};

#endif // STAGECONTROL_H
