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
 * Nautilai advanced setup window class.
 */
class AdvancedSetupDialog : public QDialog{
    Q_OBJECT

    public:
        explicit AdvancedSetupDialog(std::shared_ptr<Config> config, QWidget* parent = 0);
        ~AdvancedSetupDialog();
        void Initialize(std::vector<std::string>);
        void show();

    signals:
        void sig_ni_dev_change(std::string m_niDev, std::string m_trigDev);
        void sig_trigger_mode_change(int16_t triggerMode);
        void sig_enable_live_view_during_acquisition_change(bool enable);
        void sig_display_rois_during_live_view_change(bool enable);
        void sig_close_adv_settings();

    private slots:
        void updateAdvancedSetup();
        void on_ledDeviceList_currentTextChanged(const QString &text);
        void on_triggerDeviceList_currentTextChanged(const QString &text);
        void on_triggerModeList_currentTextChanged(const QString &text);
        void on_checkEnableLiveViewDuringAcq_stateChanged(int state);
        void on_checkDisplayRoisDuringLiveView_stateChanged(int state);
        void on_checkDownsampleRawFiles_stateChanged(int state);
        void on_checkKeepOriginalRaw_stateChanged(int state);
        void on_binFactorList_currentTextChanged(const QString &text);

    private:
        void closeEvent(QCloseEvent *event);
        void setDefaultValues();

        Ui::AdvancedSetupDialog *ui;
        std::shared_ptr<Config> m_config;
        std::string m_niDev;
        std::string m_trigDev;
        int16_t m_triggerMode;
        bool m_enableLiveViewDuringAcquisition;
        bool m_displayRoisDuringLiveView;
        bool m_enableDownsampleRawFiles;
        bool m_keepOriginalRaw;
        uint8_t m_binFactor;
};
#endif // ADVANCEDSETUPDIALOG_H
