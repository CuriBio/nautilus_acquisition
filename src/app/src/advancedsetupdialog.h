#ifndef  ADVANCEDSETUPDIALOG_H
#define  ADVANCEDSETUPDIALOG_H


#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QCloseEvent>
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
        void sig_enable_live_view_during_acquisition_change(bool enable);
        void sig_close_adv_settings();
        void sig_downsample_raw_file_changes(bool enable, int8_t binFactor, bool keepOriginal);

    private slots:
        void updateAdvancedSetup();
        void on_nidevicelist_currentTextChanged(const QString &text);
        void on_triggerModeList_currentTextChanged(const QString &text);
        void on_checkEnableLiveViewDuringAcq_stateChanged(int state);
        void on_checkDownsampleRawFiles_stateChanged(int state);
        void on_checkKeepOriginalRaw_stateChanged(int state);
        void on_binFactorList_currentTextChanged(const QString &text);

    private:
        void closeEvent(QCloseEvent *event);
        void setDefaultValues();

        Ui::AdvancedSetupDialog *ui;
        std::shared_ptr<Config> m_config;
        std::string m_niDev;
        int16_t m_triggerMode;
        bool m_enableLiveViewDuringAcquisition;
        bool m_enableDownsampleRawFiles;
        bool m_keepOriginalRaw;
        uint8_t m_binFactor;
        bool m_userConfirmed{false};
};
#endif // ADVANCEDSETUPDIALOG_H
