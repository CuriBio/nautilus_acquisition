/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created by: Qt User Interface Compiler version 6.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Settings
{
public:
    QGroupBox *groupBox;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPlainTextEdit *dirChoice;
    QToolButton *dirChoiceBtn;
    QGroupBox *groupBox_2;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *filePrefixLabel;
    QLineEdit *filePrefix;
    QLabel *fileTypeLabel;
    QComboBox *fileType;
    QDialogButtonBox *modalChoice;

    void setupUi(QDialog *Settings)
    {
        if (Settings->objectName().isEmpty())
            Settings->setObjectName("Settings");
        Settings->resize(516, 272);
        Settings->setModal(true);
        groupBox = new QGroupBox(Settings);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(20, 20, 461, 91));
        groupBox->setMinimumSize(QSize(0, 0));
        groupBox->setAutoFillBackground(true);
        groupBox->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        groupBox->setFlat(false);
        groupBox->setCheckable(false);
        horizontalLayoutWidget = new QWidget(groupBox);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(20, 40, 421, 31));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        dirChoice = new QPlainTextEdit(horizontalLayoutWidget);
        dirChoice->setObjectName("dirChoice");
        dirChoice->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        dirChoice->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        dirChoice->setTextInteractionFlags(Qt::NoTextInteraction);

        horizontalLayout->addWidget(dirChoice);

        dirChoiceBtn = new QToolButton(horizontalLayoutWidget);
        dirChoiceBtn->setObjectName("dirChoiceBtn");
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dirChoiceBtn->sizePolicy().hasHeightForWidth());
        dirChoiceBtn->setSizePolicy(sizePolicy);
        dirChoiceBtn->setMinimumSize(QSize(40, 0));
        dirChoiceBtn->setPopupMode(QToolButton::InstantPopup);
        dirChoiceBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        dirChoiceBtn->setAutoRaise(false);

        horizontalLayout->addWidget(dirChoiceBtn);

        groupBox_2 = new QGroupBox(Settings);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(20, 120, 461, 81));
        layoutWidget = new QWidget(groupBox_2);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(20, 30, 421, 31));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        filePrefixLabel = new QLabel(layoutWidget);
        filePrefixLabel->setObjectName("filePrefixLabel");
        filePrefixLabel->setScaledContents(false);

        horizontalLayout_2->addWidget(filePrefixLabel);

        filePrefix = new QLineEdit(layoutWidget);
        filePrefix->setObjectName("filePrefix");

        horizontalLayout_2->addWidget(filePrefix);

        fileTypeLabel = new QLabel(layoutWidget);
        fileTypeLabel->setObjectName("fileTypeLabel");

        horizontalLayout_2->addWidget(fileTypeLabel);

        fileType = new QComboBox(layoutWidget);
        fileType->addItem(QString());
        fileType->addItem(QString());
        fileType->addItem(QString());
        fileType->setObjectName("fileType");
        fileType->setEnabled(false);

        horizontalLayout_2->addWidget(fileType);

        modalChoice = new QDialogButtonBox(Settings);
        modalChoice->setObjectName("modalChoice");
        modalChoice->setGeometry(QRect(180, 220, 161, 41));
        modalChoice->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Save);
        modalChoice->setCenterButtons(true);

        retranslateUi(Settings);

        QMetaObject::connectSlotsByName(Settings);
    } // setupUi

    void retranslateUi(QDialog *Settings)
    {
        Settings->setWindowTitle(QCoreApplication::translate("Settings", "Dialog", nullptr));
        groupBox->setTitle(QCoreApplication::translate("Settings", "Save files to:", nullptr));
        dirChoice->setPlainText(QString());
        dirChoice->setPlaceholderText(QString());
        dirChoiceBtn->setText(QCoreApplication::translate("Settings", "...", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("Settings", "Options:", nullptr));
        filePrefixLabel->setText(QCoreApplication::translate("Settings", "Prefix:", nullptr));
        fileTypeLabel->setText(QCoreApplication::translate("Settings", "File type:", nullptr));
        fileType->setItemText(0, QCoreApplication::translate("Settings", "Tiff", nullptr));
        fileType->setItemText(1, QCoreApplication::translate("Settings", "Tiff Stack", nullptr));
        fileType->setItemText(2, QCoreApplication::translate("Settings", "BigTiff", nullptr));

        fileType->setCurrentText(QCoreApplication::translate("Settings", "Tiff", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Settings: public Ui_Settings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
