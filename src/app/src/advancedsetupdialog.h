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
        explicit AdvancedSetupDialog(toml::value config, QWidget* parent = 0);
        ~AdvancedSetupDialog();
        void Initialize(std::vector<std::string>);

    signals:
        void sig_ni_dev_change(std::string new_m_nidev);

    private slots:
        void on_confirm_new_advanced_setup();
        void nidevice_indexChanged(int index);

    private:
        Ui::AdvancedSetupDialog *ui;
        std::string m_niDev; //NI-DAQmx device name
};
#endif // ADVANCEDSETUPDIALOG_H