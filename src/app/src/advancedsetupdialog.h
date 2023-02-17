#ifndef  ADVANCEDSETUPDIALOG_H
#define  ADVANCEDSETUPDIALOG_H

#include <toml.hpp>

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <toml.hpp>

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
        std::string new_niDev;
};
#endif // ADVANCEDSETUPDIALOG_H