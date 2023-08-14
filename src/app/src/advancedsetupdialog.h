#ifndef  ADVANCEDSETUPDIALOG_H
#define  ADVANCEDSETUPDIALOG_H


#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <toml.hpp>

#include "config.h"
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
        explicit AdvancedSetupDialog(std::shared_ptr<Config> config, QWidget* parent = 0);
        ~AdvancedSetupDialog();
        void Initialize(std::vector<std::string>);

    signals:
        void sig_ni_dev_change(std::string new_m_nidev);
        void sig_trigger_mode_change(int16_t triggerMode);

    private slots:
        void update_advanced_setup();
        void on_nidevice_currentTextChanged(const QString &text);
        void on_triggerModeList_currentTextChanged(const QString &text);

    private:
        Ui::AdvancedSetupDialog *ui;
        std::shared_ptr<Config> m_config;
        std::string m_niDev;
        int16_t m_triggerMode;
};
#endif // ADVANCEDSETUPDIALOG_H
