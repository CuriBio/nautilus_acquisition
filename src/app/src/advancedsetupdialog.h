#ifndef  ADVANCEDSETUPDIALOG_H
#define  ADVANCEDSETUPDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>

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
        void Initialize(std::vector<char>,std::string* m_niDev);

    private slots:
        void on_confirm_new_advanced_setup();
        void indexChanged(int index);

    private:
        Ui::AdvancedSetupDialog *ui;
        std::string* m_niDev;
        std::string new_niDev;
};
#endif // ADVANCEDSETUPDIALOG_H