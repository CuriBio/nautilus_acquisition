#ifndef  ADVANCEDSETUPDIALOG_H
#define  ADVANCEDSETUPDIALOG_H


#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <toml.hpp>

#include <NIDAQmx_wrapper.h>

namespace Ui {
class AdvancedSetupDialog;
}
/*
 * Nautilus advanced setup window class.
 */
class AdvancedSetupDialog : public QDialog{
    Q_OBJECT

    public:
        explicit AdvancedSetupDialog(QWidget* parent = 0);
        ~AdvancedSetupDialog();
        void Initialize(std::vector<std::string>);

    private slots:
        void on_confirm_new_advanced_setup();
        void nidevice_indexChanged(int index);

    private:
        Ui::AdvancedSetupDialog *ui;

        NIDAQmx new_DAQmx; //NI-DAQmx controller for LEDs
        std::string new_niDev; //NI-DAQmx device name
        std::string new_taskAO, new_devAO;
        std::string new_taskDO, new_devDO;
};
#endif // ADVANCEDSETUPDIALOG_H