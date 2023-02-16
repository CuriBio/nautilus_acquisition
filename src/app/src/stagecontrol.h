#ifndef STAGECONTROL_H
#define STAGECONTROL_H

#define STAGE_MOVE1 5
#define STAGE_MOVE2 15
#define STAGE_MOVE3 25

#include <vector>

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <toml.hpp>

#include <TangoStage.h>


namespace Ui {
class StageControl;
}

class StagePosition : public QListWidgetItem  {
    public:
        int pos;
        double x;
        double y;

    public:
        StagePosition(int pos, double xPos, double yPos) : QListWidgetItem(fmt::format("pos_{} - x: {}, y: {}", pos, xPos, yPos).c_str()) {
            pos = pos;
            x = xPos;
            y = yPos;
        }

        ~StagePosition() {}
};

class StageControl : public QDialog {
    Q_OBJECT

    public:
        explicit StageControl(std::string comPort, QWidget *parent = nullptr);
        ~StageControl();

        void SetRelativePosition(double x, double y);
        void SetAbsolutePosition(double x, double y);

        void SetRelativeX(double x);
        void SetAbsoluteX(double x);

        void SetRelativeY(double y);
        void SetAbsoluteY(double y);

        void AddCurrentPosition();

        const std::vector<StagePosition*>& GetPositions() const;

    private slots:
        void on_addBtn_clicked();
        void on_deleteBtn_clicked();

        void on_saveListBtn_clicked();
        void on_loadListBtn_clicked();

        void on_gotoPosBtn_clicked();

        void on_stageRightBtn1_clicked() { SetRelativeX(STAGE_MOVE1); };
        void on_stageRightBtn2_clicked() { SetRelativeX(STAGE_MOVE2); };
        void on_stageRightBtn3_clicked() { SetRelativeX(STAGE_MOVE3); };

        void on_stageLeftBtn1_clicked() { SetRelativeX(-STAGE_MOVE1); };
        void on_stageLeftBtn2_clicked() { SetRelativeX(-STAGE_MOVE2); };
        void on_stageLeftBtn3_clicked() { SetRelativeX(-STAGE_MOVE3); };

        void on_stageUpBtn1_clicked() { SetRelativeY(STAGE_MOVE1); };
        void on_stageUpBtn2_clicked() { SetRelativeY(STAGE_MOVE2); };
        void on_stageUpBtn3_clicked() { SetRelativeY(STAGE_MOVE3); };

        void on_stageDownBtn1_clicked() { SetRelativeY(-STAGE_MOVE1); };
        void on_stageDownBtn2_clicked() { SetRelativeY(-STAGE_MOVE2); };
        void on_stageDownBtn3_clicked() { SetRelativeY(-STAGE_MOVE3); };

    private:
        Ui::StageControl *ui;
        std::vector<StagePosition*> m_positions;

        TangoStage* m_tango;
        std::string m_comPort;

        double m_curX{0.0}, m_curY{0.0};
};

#endif // STAGECONTROL_H
