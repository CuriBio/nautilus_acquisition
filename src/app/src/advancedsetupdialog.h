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
        explicit AdvancedSetupDialog(toml::value config,std::string* m_niDev, QWidget* parent = 0);
        ~AdvancedSetupDialog();
        void Initialize(std::vector<std::string>);
        std::string Get_Device_Selected();

    signals:
        void sig_ni_dev_change();

    private slots:
        void on_confirm_new_advanced_setup();
        void nidevice_indexChanged(int index);

    private:
        Ui::AdvancedSetupDialog *ui;
        std::string new_niDev; //NI-DAQmx device name
        std::string* m_niDev;
};
#endif // ADVANCEDSETUPDIALOG_H