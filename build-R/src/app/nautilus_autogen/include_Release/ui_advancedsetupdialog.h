/********************************************************************************
** Form generated from reading UI file 'advancedsetupdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADVANCEDSETUPDIALOG_H
#define UI_ADVANCEDSETUPDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_AdvancedSetupDialog
{
public:
    QComboBox *ledDeviceList;
    QLabel *label;
    QPushButton *updateSetupBtn;
    QLabel *label_2;
    QComboBox *triggerModeList;
    QCheckBox *checkEnableLiveViewDuringAcq;
    QLabel *label_3;
    QLabel *label_4;
    QCheckBox *checkDownsampleRawFiles;
    QLabel *label_5;
    QComboBox *binFactorList;
    QLabel *label_6;
    QCheckBox *checkKeepOriginalRaw;
    QLabel *label_7;
    QComboBox *triggerDeviceList;

    void setupUi(QDialog *AdvancedSetupDialog)
    {
        if (AdvancedSetupDialog->objectName().isEmpty())
            AdvancedSetupDialog->setObjectName("AdvancedSetupDialog");
        AdvancedSetupDialog->resize(532, 310);
        ledDeviceList = new QComboBox(AdvancedSetupDialog);
        ledDeviceList->setObjectName("ledDeviceList");
        ledDeviceList->setGeometry(QRect(310, 40, 161, 21));
        label = new QLabel(AdvancedSetupDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(120, 40, 151, 21));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        updateSetupBtn = new QPushButton(AdvancedSetupDialog);
        updateSetupBtn->setObjectName("updateSetupBtn");
        updateSetupBtn->setGeometry(QRect(190, 260, 150, 30));
        label_2 = new QLabel(AdvancedSetupDialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(160, 100, 111, 21));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        triggerModeList = new QComboBox(AdvancedSetupDialog);
        triggerModeList->setObjectName("triggerModeList");
        triggerModeList->setGeometry(QRect(310, 100, 161, 22));
        checkEnableLiveViewDuringAcq = new QCheckBox(AdvancedSetupDialog);
        checkEnableLiveViewDuringAcq->setObjectName("checkEnableLiveViewDuringAcq");
        checkEnableLiveViewDuringAcq->setGeometry(QRect(310, 130, 21, 21));
        checkEnableLiveViewDuringAcq->setLayoutDirection(Qt::RightToLeft);
        label_3 = new QLabel(AdvancedSetupDialog);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(10, 130, 261, 21));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_4 = new QLabel(AdvancedSetupDialog);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(10, 160, 261, 21));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        checkDownsampleRawFiles = new QCheckBox(AdvancedSetupDialog);
        checkDownsampleRawFiles->setObjectName("checkDownsampleRawFiles");
        checkDownsampleRawFiles->setGeometry(QRect(310, 160, 21, 21));
        checkDownsampleRawFiles->setLayoutDirection(Qt::RightToLeft);
        label_5 = new QLabel(AdvancedSetupDialog);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 190, 261, 21));
        label_5->setStyleSheet(QString::fromUtf8("font-size: 12px;"));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        binFactorList = new QComboBox(AdvancedSetupDialog);
        binFactorList->addItem(QString());
        binFactorList->addItem(QString());
        binFactorList->addItem(QString());
        binFactorList->addItem(QString());
        binFactorList->setObjectName("binFactorList");
        binFactorList->setEnabled(true);
        binFactorList->setGeometry(QRect(310, 190, 91, 22));
        label_6 = new QLabel(AdvancedSetupDialog);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(10, 220, 261, 21));
        label_6->setStyleSheet(QString::fromUtf8("font-size: 12px;"));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        checkKeepOriginalRaw = new QCheckBox(AdvancedSetupDialog);
        checkKeepOriginalRaw->setObjectName("checkKeepOriginalRaw");
        checkKeepOriginalRaw->setGeometry(QRect(310, 220, 21, 21));
        checkKeepOriginalRaw->setLayoutDirection(Qt::RightToLeft);
        label_7 = new QLabel(AdvancedSetupDialog);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(100, 70, 171, 21));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        triggerDeviceList = new QComboBox(AdvancedSetupDialog);
        triggerDeviceList->setObjectName("triggerDeviceList");
        triggerDeviceList->setGeometry(QRect(310, 70, 161, 21));

        retranslateUi(AdvancedSetupDialog);

        QMetaObject::connectSlotsByName(AdvancedSetupDialog);
    } // setupUi

    void retranslateUi(QDialog *AdvancedSetupDialog)
    {
        AdvancedSetupDialog->setWindowTitle(QCoreApplication::translate("AdvancedSetupDialog", "Advanced Setup", nullptr));
        label->setText(QCoreApplication::translate("AdvancedSetupDialog", "Select LED NI Device:", nullptr));
        updateSetupBtn->setText(QCoreApplication::translate("AdvancedSetupDialog", "Confirm New Setup", nullptr));
        label_2->setText(QCoreApplication::translate("AdvancedSetupDialog", "Trigger In:", nullptr));
        checkEnableLiveViewDuringAcq->setText(QString());
        label_3->setText(QCoreApplication::translate("AdvancedSetupDialog", "Enable live view during acquisition:", nullptr));
        label_4->setText(QCoreApplication::translate("AdvancedSetupDialog", "Downsample raw data file:", nullptr));
        checkDownsampleRawFiles->setText(QString());
        label_5->setText(QCoreApplication::translate("AdvancedSetupDialog", "Binning Factor:", nullptr));
        binFactorList->setItemText(0, QCoreApplication::translate("AdvancedSetupDialog", "2", nullptr));
        binFactorList->setItemText(1, QCoreApplication::translate("AdvancedSetupDialog", "4", nullptr));
        binFactorList->setItemText(2, QCoreApplication::translate("AdvancedSetupDialog", "8", nullptr));
        binFactorList->setItemText(3, QCoreApplication::translate("AdvancedSetupDialog", "16", nullptr));

        label_6->setText(QCoreApplication::translate("AdvancedSetupDialog", "Keep original:", nullptr));
        checkKeepOriginalRaw->setText(QString());
        label_7->setText(QCoreApplication::translate("AdvancedSetupDialog", "Select Trigger NI Device:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AdvancedSetupDialog: public Ui_AdvancedSetupDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADVANCEDSETUPDIALOG_H
