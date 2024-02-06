/********************************************************************************
** Form generated from reading UI file 'stagecontrol.ui'
**
** Created by: Qt User Interface Compiler version 6.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STAGECONTROL_H
#define UI_STAGECONTROL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_StageControl
{
public:
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QVBoxLayout *stageLocControls;
    QPushButton *addBtn;
    QPushButton *deleteBtn;
    QPushButton *gotoPosBtn;
    QPushButton *saveListBtn;
    QPushButton *loadListBtn;
    QGridLayout *gridLayout_4;
    QWidget *stageControl;
    QWidget *stageUp;
    QVBoxLayout *verticalLayout_13;
    QPushButton *stageUpBtn3;
    QPushButton *stageUpBtn2;
    QPushButton *stageUpBtn1;
    QWidget *stageRight;
    QHBoxLayout *horizontalLayout_13;
    QPushButton *stageRightBtn1;
    QPushButton *stageRightBtn2;
    QPushButton *stageRightBtn3;
    QWidget *stageLeft;
    QHBoxLayout *horizontalLayout_14;
    QPushButton *stageLeftBtn3;
    QPushButton *stageLeftBtn2;
    QPushButton *stageLeftBtn1;
    QWidget *stageDown;
    QVBoxLayout *verticalLayout_14;
    QPushButton *stageDownBtn1;
    QPushButton *stageDownBtn2;
    QPushButton *stageDownBtn3;
    QListWidget *stageLocations;

    void setupUi(QDialog *StageControl)
    {
        if (StageControl->objectName().isEmpty())
            StageControl->setObjectName("StageControl");
        StageControl->resize(656, 627);
        gridLayoutWidget = new QWidget(StageControl);
        gridLayoutWidget->setObjectName("gridLayoutWidget");
        gridLayoutWidget->setGeometry(QRect(10, 10, 641, 611));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        stageLocControls = new QVBoxLayout();
        stageLocControls->setSpacing(10);
        stageLocControls->setObjectName("stageLocControls");
        stageLocControls->setSizeConstraint(QLayout::SetFixedSize);
        stageLocControls->setContentsMargins(-1, 10, -1, -1);
        addBtn = new QPushButton(gridLayoutWidget);
        addBtn->setObjectName("addBtn");
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(addBtn->sizePolicy().hasHeightForWidth());
        addBtn->setSizePolicy(sizePolicy);
        addBtn->setMinimumSize(QSize(0, 22));
        addBtn->setMaximumSize(QSize(16777215, 50));

        stageLocControls->addWidget(addBtn);

        deleteBtn = new QPushButton(gridLayoutWidget);
        deleteBtn->setObjectName("deleteBtn");
        sizePolicy.setHeightForWidth(deleteBtn->sizePolicy().hasHeightForWidth());
        deleteBtn->setSizePolicy(sizePolicy);
        deleteBtn->setMinimumSize(QSize(0, 50));
        deleteBtn->setMaximumSize(QSize(16777215, 50));

        stageLocControls->addWidget(deleteBtn);

        gotoPosBtn = new QPushButton(gridLayoutWidget);
        gotoPosBtn->setObjectName("gotoPosBtn");
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(gotoPosBtn->sizePolicy().hasHeightForWidth());
        gotoPosBtn->setSizePolicy(sizePolicy1);
        gotoPosBtn->setMinimumSize(QSize(0, 50));
        gotoPosBtn->setMaximumSize(QSize(16777215, 50));

        stageLocControls->addWidget(gotoPosBtn);

        saveListBtn = new QPushButton(gridLayoutWidget);
        saveListBtn->setObjectName("saveListBtn");
        sizePolicy1.setHeightForWidth(saveListBtn->sizePolicy().hasHeightForWidth());
        saveListBtn->setSizePolicy(sizePolicy1);
        saveListBtn->setMinimumSize(QSize(0, 50));
        saveListBtn->setMaximumSize(QSize(16777215, 50));

        stageLocControls->addWidget(saveListBtn);

        loadListBtn = new QPushButton(gridLayoutWidget);
        loadListBtn->setObjectName("loadListBtn");
        sizePolicy1.setHeightForWidth(loadListBtn->sizePolicy().hasHeightForWidth());
        loadListBtn->setSizePolicy(sizePolicy1);
        loadListBtn->setMinimumSize(QSize(0, 41));
        loadListBtn->setMaximumSize(QSize(16777215, 50));

        stageLocControls->addWidget(loadListBtn);

        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName("gridLayout_4");
        stageControl = new QWidget(gridLayoutWidget);
        stageControl->setObjectName("stageControl");
        stageControl->setMaximumSize(QSize(16777178, 16777215));
        stageControl->setAutoFillBackground(false);
        stageUp = new QWidget(stageControl);
        stageUp->setObjectName("stageUp");
        stageUp->setGeometry(QRect(145, 15, 36, 120));
        verticalLayout_13 = new QVBoxLayout(stageUp);
        verticalLayout_13->setSpacing(2);
        verticalLayout_13->setObjectName("verticalLayout_13");
        verticalLayout_13->setSizeConstraint(QLayout::SetFixedSize);
        verticalLayout_13->setContentsMargins(2, 2, 2, 2);
        stageUpBtn3 = new QPushButton(stageUp);
        stageUpBtn3->setObjectName("stageUpBtn3");
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(stageUpBtn3->sizePolicy().hasHeightForWidth());
        stageUpBtn3->setSizePolicy(sizePolicy2);
        stageUpBtn3->setMinimumSize(QSize(36, 36));
        stageUpBtn3->setText(QString::fromUtf8(""));
        stageUpBtn3->setAutoRepeat(true);
        stageUpBtn3->setAutoRepeatDelay(100);
        stageUpBtn3->setAutoRepeatInterval(200);
        stageUpBtn3->setAutoDefault(false);
        stageUpBtn3->setFlat(false);

        verticalLayout_13->addWidget(stageUpBtn3);

        stageUpBtn2 = new QPushButton(stageUp);
        stageUpBtn2->setObjectName("stageUpBtn2");
        sizePolicy2.setHeightForWidth(stageUpBtn2->sizePolicy().hasHeightForWidth());
        stageUpBtn2->setSizePolicy(sizePolicy2);
        stageUpBtn2->setMinimumSize(QSize(36, 36));
        stageUpBtn2->setText(QString::fromUtf8(""));
        stageUpBtn2->setAutoRepeat(true);
        stageUpBtn2->setAutoRepeatDelay(100);
        stageUpBtn2->setAutoRepeatInterval(200);
        stageUpBtn2->setAutoDefault(false);
        stageUpBtn2->setFlat(false);

        verticalLayout_13->addWidget(stageUpBtn2);

        stageUpBtn1 = new QPushButton(stageUp);
        stageUpBtn1->setObjectName("stageUpBtn1");
        sizePolicy2.setHeightForWidth(stageUpBtn1->sizePolicy().hasHeightForWidth());
        stageUpBtn1->setSizePolicy(sizePolicy2);
        stageUpBtn1->setMinimumSize(QSize(36, 36));
        stageUpBtn1->setText(QString::fromUtf8(""));
        stageUpBtn1->setAutoRepeat(true);
        stageUpBtn1->setAutoRepeatDelay(100);
        stageUpBtn1->setAutoRepeatInterval(200);
        stageUpBtn1->setAutoDefault(false);
        stageUpBtn1->setFlat(false);

        verticalLayout_13->addWidget(stageUpBtn1);

        stageRight = new QWidget(stageControl);
        stageRight->setObjectName("stageRight");
        stageRight->setGeometry(QRect(190, 130, 130, 40));
        stageRight->setMinimumSize(QSize(0, 0));
        stageRight->setMaximumSize(QSize(16777215, 16777215));
        horizontalLayout_13 = new QHBoxLayout(stageRight);
        horizontalLayout_13->setSpacing(2);
        horizontalLayout_13->setObjectName("horizontalLayout_13");
        horizontalLayout_13->setSizeConstraint(QLayout::SetFixedSize);
        horizontalLayout_13->setContentsMargins(2, 2, 2, 2);
        stageRightBtn1 = new QPushButton(stageRight);
        stageRightBtn1->setObjectName("stageRightBtn1");
        sizePolicy2.setHeightForWidth(stageRightBtn1->sizePolicy().hasHeightForWidth());
        stageRightBtn1->setSizePolicy(sizePolicy2);
        stageRightBtn1->setMinimumSize(QSize(36, 36));
        stageRightBtn1->setText(QString::fromUtf8(""));
        stageRightBtn1->setAutoRepeat(true);
        stageRightBtn1->setAutoRepeatDelay(100);
        stageRightBtn1->setAutoRepeatInterval(200);
        stageRightBtn1->setAutoDefault(false);
        stageRightBtn1->setFlat(false);

        horizontalLayout_13->addWidget(stageRightBtn1);

        stageRightBtn2 = new QPushButton(stageRight);
        stageRightBtn2->setObjectName("stageRightBtn2");
        sizePolicy2.setHeightForWidth(stageRightBtn2->sizePolicy().hasHeightForWidth());
        stageRightBtn2->setSizePolicy(sizePolicy2);
        stageRightBtn2->setMinimumSize(QSize(36, 36));
        stageRightBtn2->setText(QString::fromUtf8(""));
        stageRightBtn2->setAutoRepeat(true);
        stageRightBtn2->setAutoRepeatDelay(100);
        stageRightBtn2->setAutoRepeatInterval(200);
        stageRightBtn2->setAutoDefault(false);
        stageRightBtn2->setFlat(false);

        horizontalLayout_13->addWidget(stageRightBtn2);

        stageRightBtn3 = new QPushButton(stageRight);
        stageRightBtn3->setObjectName("stageRightBtn3");
        sizePolicy2.setHeightForWidth(stageRightBtn3->sizePolicy().hasHeightForWidth());
        stageRightBtn3->setSizePolicy(sizePolicy2);
        stageRightBtn3->setMinimumSize(QSize(36, 36));
        stageRightBtn3->setText(QString::fromUtf8(""));
        stageRightBtn3->setAutoRepeat(true);
        stageRightBtn3->setAutoRepeatDelay(100);
        stageRightBtn3->setAutoRepeatInterval(200);
        stageRightBtn3->setAutoDefault(false);
        stageRightBtn3->setFlat(false);

        horizontalLayout_13->addWidget(stageRightBtn3);

        stageLeft = new QWidget(stageControl);
        stageLeft->setObjectName("stageLeft");
        stageLeft->setGeometry(QRect(23, 130, 130, 40));
        horizontalLayout_14 = new QHBoxLayout(stageLeft);
        horizontalLayout_14->setSpacing(2);
        horizontalLayout_14->setObjectName("horizontalLayout_14");
        horizontalLayout_14->setSizeConstraint(QLayout::SetFixedSize);
        horizontalLayout_14->setContentsMargins(2, 2, 2, 2);
        stageLeftBtn3 = new QPushButton(stageLeft);
        stageLeftBtn3->setObjectName("stageLeftBtn3");
        sizePolicy2.setHeightForWidth(stageLeftBtn3->sizePolicy().hasHeightForWidth());
        stageLeftBtn3->setSizePolicy(sizePolicy2);
        stageLeftBtn3->setMinimumSize(QSize(36, 36));
        stageLeftBtn3->setText(QString::fromUtf8(""));
        stageLeftBtn3->setAutoRepeat(true);
        stageLeftBtn3->setAutoRepeatDelay(100);
        stageLeftBtn3->setAutoRepeatInterval(200);
        stageLeftBtn3->setAutoDefault(false);
        stageLeftBtn3->setFlat(false);

        horizontalLayout_14->addWidget(stageLeftBtn3);

        stageLeftBtn2 = new QPushButton(stageLeft);
        stageLeftBtn2->setObjectName("stageLeftBtn2");
        sizePolicy2.setHeightForWidth(stageLeftBtn2->sizePolicy().hasHeightForWidth());
        stageLeftBtn2->setSizePolicy(sizePolicy2);
        stageLeftBtn2->setMinimumSize(QSize(36, 36));
        stageLeftBtn2->setText(QString::fromUtf8(""));
        stageLeftBtn2->setAutoRepeat(true);
        stageLeftBtn2->setAutoRepeatDelay(100);
        stageLeftBtn2->setAutoRepeatInterval(200);
        stageLeftBtn2->setAutoDefault(false);
        stageLeftBtn2->setFlat(false);

        horizontalLayout_14->addWidget(stageLeftBtn2);

        stageLeftBtn1 = new QPushButton(stageLeft);
        stageLeftBtn1->setObjectName("stageLeftBtn1");
        sizePolicy2.setHeightForWidth(stageLeftBtn1->sizePolicy().hasHeightForWidth());
        stageLeftBtn1->setSizePolicy(sizePolicy2);
        stageLeftBtn1->setMinimumSize(QSize(36, 36));
        stageLeftBtn1->setText(QString::fromUtf8(""));
        stageLeftBtn1->setAutoRepeat(true);
        stageLeftBtn1->setAutoRepeatDelay(100);
        stageLeftBtn1->setAutoRepeatInterval(200);
        stageLeftBtn1->setAutoDefault(false);
        stageLeftBtn1->setFlat(false);

        horizontalLayout_14->addWidget(stageLeftBtn1);

        stageDown = new QWidget(stageControl);
        stageDown->setObjectName("stageDown");
        stageDown->setGeometry(QRect(145, 165, 36, 120));
        verticalLayout_14 = new QVBoxLayout(stageDown);
        verticalLayout_14->setSpacing(2);
        verticalLayout_14->setObjectName("verticalLayout_14");
        verticalLayout_14->setSizeConstraint(QLayout::SetFixedSize);
        verticalLayout_14->setContentsMargins(2, 2, 2, 2);
        stageDownBtn1 = new QPushButton(stageDown);
        stageDownBtn1->setObjectName("stageDownBtn1");
        sizePolicy2.setHeightForWidth(stageDownBtn1->sizePolicy().hasHeightForWidth());
        stageDownBtn1->setSizePolicy(sizePolicy2);
        stageDownBtn1->setMinimumSize(QSize(36, 36));
        stageDownBtn1->setText(QString::fromUtf8(""));
        stageDownBtn1->setAutoRepeat(true);
        stageDownBtn1->setAutoRepeatDelay(100);
        stageDownBtn1->setAutoRepeatInterval(200);
        stageDownBtn1->setAutoDefault(false);
        stageDownBtn1->setFlat(false);

        verticalLayout_14->addWidget(stageDownBtn1);

        stageDownBtn2 = new QPushButton(stageDown);
        stageDownBtn2->setObjectName("stageDownBtn2");
        sizePolicy2.setHeightForWidth(stageDownBtn2->sizePolicy().hasHeightForWidth());
        stageDownBtn2->setSizePolicy(sizePolicy2);
        stageDownBtn2->setMinimumSize(QSize(36, 36));
        stageDownBtn2->setText(QString::fromUtf8(""));
        stageDownBtn2->setAutoRepeat(true);
        stageDownBtn2->setAutoRepeatDelay(100);
        stageDownBtn2->setAutoRepeatInterval(200);
        stageDownBtn2->setAutoDefault(false);
        stageDownBtn2->setFlat(false);

        verticalLayout_14->addWidget(stageDownBtn2);

        stageDownBtn3 = new QPushButton(stageDown);
        stageDownBtn3->setObjectName("stageDownBtn3");
        sizePolicy2.setHeightForWidth(stageDownBtn3->sizePolicy().hasHeightForWidth());
        stageDownBtn3->setSizePolicy(sizePolicy2);
        stageDownBtn3->setMinimumSize(QSize(36, 36));
        stageDownBtn3->setText(QString::fromUtf8(""));
        stageDownBtn3->setAutoRepeat(true);
        stageDownBtn3->setAutoRepeatDelay(100);
        stageDownBtn3->setAutoRepeatInterval(200);
        stageDownBtn3->setAutoDefault(false);
        stageDownBtn3->setFlat(false);

        verticalLayout_14->addWidget(stageDownBtn3);


        gridLayout_4->addWidget(stageControl, 0, 0, 1, 1);


        stageLocControls->addLayout(gridLayout_4);


        gridLayout->addLayout(stageLocControls, 0, 1, 1, 1);

        stageLocations = new QListWidget(gridLayoutWidget);
        stageLocations->setObjectName("stageLocations");
        stageLocations->setMaximumSize(QSize(300, 16777215));
        stageLocations->setStyleSheet(QString::fromUtf8("font-size: 16px;"));
        stageLocations->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
        stageLocations->setDragDropOverwriteMode(false);
        stageLocations->setUniformItemSizes(true);
        stageLocations->setItemAlignment(Qt::AlignLeading);

        gridLayout->addWidget(stageLocations, 0, 0, 1, 1);


        retranslateUi(StageControl);

        QMetaObject::connectSlotsByName(StageControl);
    } // setupUi

    void retranslateUi(QDialog *StageControl)
    {
        StageControl->setWindowTitle(QCoreApplication::translate("StageControl", "Dialog", nullptr));
        addBtn->setText(QCoreApplication::translate("StageControl", "Add Position", nullptr));
        deleteBtn->setText(QCoreApplication::translate("StageControl", "Delete Position", nullptr));
        gotoPosBtn->setText(QCoreApplication::translate("StageControl", "Goto Position", nullptr));
        saveListBtn->setText(QCoreApplication::translate("StageControl", "Save Position List", nullptr));
        loadListBtn->setText(QCoreApplication::translate("StageControl", "Load Position List", nullptr));
#if QT_CONFIG(accessibility)
        stageUpBtn3->setAccessibleName(QCoreApplication::translate("StageControl", "stageUpBtn3", nullptr));
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        stageUpBtn2->setAccessibleName(QCoreApplication::translate("StageControl", "stageUpBtn2", nullptr));
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        stageUpBtn1->setAccessibleName(QCoreApplication::translate("StageControl", "stageUpBtn1", nullptr));
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        stageRightBtn1->setAccessibleName(QCoreApplication::translate("StageControl", "stageRightBtn1", nullptr));
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        stageRightBtn2->setAccessibleName(QCoreApplication::translate("StageControl", "stageRightBtn2", nullptr));
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        stageRightBtn3->setAccessibleName(QCoreApplication::translate("StageControl", "stageRightBtn3", nullptr));
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        stageLeftBtn3->setAccessibleName(QCoreApplication::translate("StageControl", "stageLeftBtn3", nullptr));
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        stageLeftBtn2->setAccessibleName(QCoreApplication::translate("StageControl", "stageLeftBtn2", nullptr));
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        stageLeftBtn1->setAccessibleName(QCoreApplication::translate("StageControl", "stageLeftBtn1", nullptr));
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        stageDownBtn1->setAccessibleName(QCoreApplication::translate("StageControl", "stageDownBtn1", nullptr));
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        stageDownBtn2->setAccessibleName(QCoreApplication::translate("StageControl", "stageDownBtn2", nullptr));
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        stageDownBtn3->setAccessibleName(QCoreApplication::translate("StageControl", "stageDownBtn3", nullptr));
#endif // QT_CONFIG(accessibility)
    } // retranslateUi

};

namespace Ui {
    class StageControl: public Ui_StageControl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STAGECONTROL_H
