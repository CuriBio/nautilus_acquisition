#ifndef SETTINGS_H
#define SETTINGS_H
#include <filesystem>
#include <string>

#include <QDialog>
#include <QWidget>

#include "ui_settings.h"

class Settings : public QDialog {
    Q_OBJECT

    public:
        explicit Settings(QWidget* parent, std::filesystem::path path, std::string prefix);
        virtual ~Settings();

    private:
        Ui::Settings ui;

    signals:
        void sig_settings_changed(std::string dir, std::string prefix);

    private slots:
        void on_dirChoiceBtn_clicked();
        void on_modalChoice_accepted();
        void on_modalChoice_rejected();
};

#endif //SETTINGS_H
