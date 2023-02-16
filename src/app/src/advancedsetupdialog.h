#ifndef  ADVANCEDSETUPDIALOG_H
#define  ADVANCEDSETUPDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>

namespace Ui {
class AdvancedSetupDialog;
}

class AdvancedSetupDialog : public QDialog{
    Q_OBJECT

public:
    explicit AdvancedSetupDialog(QWidget *parent = 0);
    ~AdvancedSetupDialog();

private:
    Ui::AdvancedSetupDialog *ui;
};
#endif // ADVANCEDSETUPDIALOG_H