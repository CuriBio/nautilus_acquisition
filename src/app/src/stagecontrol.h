#ifndef STAGECONTROL_H
#define STAGECONTROL_H

#include <vector>

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <toml.hpp>

#include <TangoStage.h>

namespace Ui {
class StageControl;
}

class LocationData : public QListWidgetItem  {
    public:
        int pos;
        double x;
        double y;

    public:
        LocationData(int pos, double xPos, double yPos) : QListWidgetItem(fmt::format("pos_{} - x: {}, y: {}", pos, xPos, yPos).c_str()) {
            pos = pos;
            x = xPos;
            y = yPos;
        }

        ~LocationData() {}
};

class StageControl : public QDialog {
    Q_OBJECT

    public:
        explicit StageControl(QWidget *parent = nullptr);
        ~StageControl();

        void SetRelativePosition(double x, double y);
        void SetAbsolutePosition(double x, double y);

        void SetRelativeX(double x);
        void SetAbsoluteX(double x);

        void SetRelativeY(double y);
        void SetAbsoluteY(double y);

        const std::vector<LocationData*>& GetLocations() const;

    private slots:
        void on_addBtn_clicked();
        void on_deleteBtn_clicked();

        void on_saveListBtn_clicked();
        void on_loadListBtn_clicked();

        void on_gotoPosBtn_clicked();

    private:
        Ui::StageControl *ui;
        std::vector<LocationData*> m_locations;

        TangoStage* m_tango;

        double m_curX{0.0}, m_curY{0.0};
};

#endif // STAGECONTROL_H
