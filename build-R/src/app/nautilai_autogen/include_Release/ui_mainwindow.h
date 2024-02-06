/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "histview.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QFrame *rightframe;
    QGridLayout *gridLayout_3;
    QSpacerItem *verticalSpacer_3;
    QWidget *widget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *platemapLayout;
    QComboBox *plateFormatDropDown;
    QGridLayout *liveViewLayout;
    QLabel *levelMax;
    QLabel *levelMin;
    QFrame *settings_frame;
    QGridLayout *gridLayout_7;
    QGridLayout *gridLayout_6;
    QPushButton *startAcquisitionBtn;
    QPushButton *advancedSetupBtn;
    QPushButton *liveScanBtn;
    QPushButton *stageNavigationBtn;
    QFrame *dataTypeFrame;
    QComboBox *dataTypeList;
    QPushButton *settingsBtn;
    QGridLayout *gridLayout_4;
    QFrame *frame_3;
    QGridLayout *gridLayout_5;
    QGridLayout *gridLayout;
    QLabel *ledIntensityLabel;
    QLabel *frameRateFPSLabel;
    QLabel *durationSLabel;
    QDoubleSpinBox *durationEdit;
    QDoubleSpinBox *frameRateEdit;
    QDoubleSpinBox *ledIntensityEdit;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *histLayout;
    HistView *histView;
    QSlider *levelsSlider;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1800, 1000);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(1800, 1000));
        MainWindow->setBaseSize(QSize(1800, 1000));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setBaseSize(QSize(1800, 1000));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setSizeConstraint(QLayout::SetMinimumSize);
        gridLayout_2->setContentsMargins(12, -1, -1, -1);
        rightframe = new QFrame(centralwidget);
        rightframe->setObjectName("rightframe");
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(rightframe->sizePolicy().hasHeightForWidth());
        rightframe->setSizePolicy(sizePolicy1);
        rightframe->setMinimumSize(QSize(455, 710));
        rightframe->setMaximumSize(QSize(455, 16777215));
        rightframe->setBaseSize(QSize(455, 710));
        rightframe->setAutoFillBackground(false);
        rightframe->setFrameShape(QFrame::Box);
        rightframe->setFrameShadow(QFrame::Sunken);
        gridLayout_3 = new QGridLayout(rightframe);
        gridLayout_3->setObjectName("gridLayout_3");
        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer_3, 2, 0, 1, 1);

        widget = new QWidget(rightframe);
        widget->setObjectName("widget");
        widget->setMinimumSize(QSize(431, 280));
        widget->setStyleSheet(QString::fromUtf8("background-color: #1C1C1C;"));
        verticalLayoutWidget = new QWidget(widget);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(10, 10, 411, 261));
        platemapLayout = new QVBoxLayout(verticalLayoutWidget);
        platemapLayout->setObjectName("platemapLayout");
        platemapLayout->setContentsMargins(0, 0, 0, 0);

        gridLayout_3->addWidget(widget, 1, 0, 1, 1);

        plateFormatDropDown = new QComboBox(rightframe);
        plateFormatDropDown->setObjectName("plateFormatDropDown");

        gridLayout_3->addWidget(plateFormatDropDown, 0, 0, 1, 1);


        gridLayout_2->addWidget(rightframe, 0, 6, 3, 1);

        liveViewLayout = new QGridLayout();
        liveViewLayout->setObjectName("liveViewLayout");
        liveViewLayout->setSizeConstraint(QLayout::SetMinimumSize);

        gridLayout_2->addLayout(liveViewLayout, 0, 1, 2, 5);

        levelMax = new QLabel(centralwidget);
        levelMax->setObjectName("levelMax");
        levelMax->setMinimumSize(QSize(35, 118));
        levelMax->setMaximumSize(QSize(16777215, 115));
        levelMax->setStyleSheet(QString::fromUtf8("margin-left: "));
        levelMax->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        levelMax->setMargin(0);
        levelMax->setIndent(-1);

        gridLayout_2->addWidget(levelMax, 2, 4, 1, 1);

        levelMin = new QLabel(centralwidget);
        levelMin->setObjectName("levelMin");
        levelMin->setMinimumSize(QSize(35, 118));
        levelMin->setMaximumSize(QSize(16777215, 119));
        levelMin->setAlignment(Qt::AlignBottom|Qt::AlignRight|Qt::AlignTrailing);

        gridLayout_2->addWidget(levelMin, 2, 1, 1, 1);

        settings_frame = new QFrame(centralwidget);
        settings_frame->setObjectName("settings_frame");
        sizePolicy1.setHeightForWidth(settings_frame->sizePolicy().hasHeightForWidth());
        settings_frame->setSizePolicy(sizePolicy1);
        settings_frame->setMinimumSize(QSize(455, 710));
        settings_frame->setMaximumSize(QSize(455, 16777215));
        settings_frame->setBaseSize(QSize(455, 710));
        settings_frame->setAutoFillBackground(false);
        settings_frame->setFrameShape(QFrame::Box);
        settings_frame->setFrameShadow(QFrame::Sunken);
        gridLayout_7 = new QGridLayout(settings_frame);
        gridLayout_7->setObjectName("gridLayout_7");
        gridLayout_6 = new QGridLayout();
        gridLayout_6->setObjectName("gridLayout_6");
        gridLayout_6->setVerticalSpacing(18);
        gridLayout_6->setContentsMargins(-1, 0, -1, -1);
        startAcquisitionBtn = new QPushButton(settings_frame);
        startAcquisitionBtn->setObjectName("startAcquisitionBtn");
        startAcquisitionBtn->setMinimumSize(QSize(0, 60));

        gridLayout_6->addWidget(startAcquisitionBtn, 5, 0, 1, 1);

        advancedSetupBtn = new QPushButton(settings_frame);
        advancedSetupBtn->setObjectName("advancedSetupBtn");
        advancedSetupBtn->setMinimumSize(QSize(0, 60));
        advancedSetupBtn->setFlat(false);

        gridLayout_6->addWidget(advancedSetupBtn, 1, 0, 1, 1);

        liveScanBtn = new QPushButton(settings_frame);
        liveScanBtn->setObjectName("liveScanBtn");
        liveScanBtn->setMinimumSize(QSize(0, 60));

        gridLayout_6->addWidget(liveScanBtn, 0, 0, 1, 1);

        stageNavigationBtn = new QPushButton(settings_frame);
        stageNavigationBtn->setObjectName("stageNavigationBtn");
        stageNavigationBtn->setMinimumSize(QSize(0, 60));

        gridLayout_6->addWidget(stageNavigationBtn, 6, 0, 1, 1);

        dataTypeFrame = new QFrame(settings_frame);
        dataTypeFrame->setObjectName("dataTypeFrame");
        dataTypeFrame->setMinimumSize(QSize(0, 30));
        dataTypeFrame->setFrameShape(QFrame::StyledPanel);
        dataTypeFrame->setFrameShadow(QFrame::Raised);
        dataTypeList = new QComboBox(dataTypeFrame);
        dataTypeList->setObjectName("dataTypeList");
        dataTypeList->setGeometry(QRect(80, 0, 275, 32));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(dataTypeList->sizePolicy().hasHeightForWidth());
        dataTypeList->setSizePolicy(sizePolicy2);
        dataTypeList->setBaseSize(QSize(0, 0));

        gridLayout_6->addWidget(dataTypeFrame, 2, 0, 1, 1);

        settingsBtn = new QPushButton(settings_frame);
        settingsBtn->setObjectName("settingsBtn");
        settingsBtn->setMinimumSize(QSize(0, 60));

        gridLayout_6->addWidget(settingsBtn, 4, 0, 1, 1);


        gridLayout_7->addLayout(gridLayout_6, 1, 0, 1, 1);

        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName("gridLayout_4");
        gridLayout_4->setContentsMargins(75, -1, 75, -1);

        gridLayout_7->addLayout(gridLayout_4, 2, 0, 1, 1);

        frame_3 = new QFrame(settings_frame);
        frame_3->setObjectName("frame_3");
        sizePolicy2.setHeightForWidth(frame_3->sizePolicy().hasHeightForWidth());
        frame_3->setSizePolicy(sizePolicy2);
        frame_3->setMinimumSize(QSize(200, 200));
        frame_3->setMaximumSize(QSize(16777215, 200));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        gridLayout_5 = new QGridLayout(frame_3);
        gridLayout_5->setObjectName("gridLayout_5");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        ledIntensityLabel = new QLabel(frame_3);
        ledIntensityLabel->setObjectName("ledIntensityLabel");
        ledIntensityLabel->setMinimumSize(QSize(0, 40));

        gridLayout->addWidget(ledIntensityLabel, 0, 0, 1, 1);

        frameRateFPSLabel = new QLabel(frame_3);
        frameRateFPSLabel->setObjectName("frameRateFPSLabel");
        frameRateFPSLabel->setMinimumSize(QSize(0, 40));

        gridLayout->addWidget(frameRateFPSLabel, 1, 0, 1, 1);

        durationSLabel = new QLabel(frame_3);
        durationSLabel->setObjectName("durationSLabel");
        durationSLabel->setMinimumSize(QSize(0, 40));
        durationSLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(durationSLabel, 2, 0, 1, 1);

        durationEdit = new QDoubleSpinBox(frame_3);
        durationEdit->setObjectName("durationEdit");
        durationEdit->setMinimumSize(QSize(0, 40));
        durationEdit->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        durationEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
        durationEdit->setMaximum(10000.000000000000000);
        durationEdit->setSingleStep(0.100000000000000);
        durationEdit->setStepType(QAbstractSpinBox::DefaultStepType);

        gridLayout->addWidget(durationEdit, 2, 1, 1, 1);

        frameRateEdit = new QDoubleSpinBox(frame_3);
        frameRateEdit->setObjectName("frameRateEdit");
        frameRateEdit->setMinimumSize(QSize(0, 40));
        frameRateEdit->setAutoFillBackground(true);
        frameRateEdit->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        frameRateEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
        frameRateEdit->setMaximum(500.000000000000000);

        gridLayout->addWidget(frameRateEdit, 1, 1, 1, 1);

        ledIntensityEdit = new QDoubleSpinBox(frame_3);
        ledIntensityEdit->setObjectName("ledIntensityEdit");
        ledIntensityEdit->setMinimumSize(QSize(0, 40));
        ledIntensityEdit->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        ledIntensityEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
        ledIntensityEdit->setMaximum(100.000000000000000);
        ledIntensityEdit->setSingleStep(0.100000000000000);

        gridLayout->addWidget(ledIntensityEdit, 0, 1, 1, 1);


        gridLayout_5->addLayout(gridLayout, 0, 0, 1, 1);


        gridLayout_7->addWidget(frame_3, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_7->addItem(verticalSpacer, 3, 0, 1, 1);


        gridLayout_2->addWidget(settings_frame, 0, 0, 3, 1);

        histLayout = new QVBoxLayout();
        histLayout->setSpacing(5);
        histLayout->setObjectName("histLayout");
        histLayout->setSizeConstraint(QLayout::SetFixedSize);
        histView = new HistView(centralwidget);
        histView->setObjectName("histView");
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(histView->sizePolicy().hasHeightForWidth());
        histView->setSizePolicy(sizePolicy3);
        histView->setMinimumSize(QSize(768, 100));
        histView->setMaximumSize(QSize(768, 100));
        histView->setBaseSize(QSize(768, 100));
        histView->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 120, 110);"));

        histLayout->addWidget(histView);

        levelsSlider = new QSlider(centralwidget);
        levelsSlider->setObjectName("levelsSlider");
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(levelsSlider->sizePolicy().hasHeightForWidth());
        levelsSlider->setSizePolicy(sizePolicy4);
        levelsSlider->setMinimumSize(QSize(768, 25));
        levelsSlider->setMaximumSize(QSize(768, 25));
        levelsSlider->setMaximum(4095);
        levelsSlider->setSingleStep(1);
        levelsSlider->setPageStep(1);
        levelsSlider->setValue(4095);
        levelsSlider->setOrientation(Qt::Horizontal);
        levelsSlider->setTickPosition(QSlider::NoTicks);

        histLayout->addWidget(levelsSlider);


        gridLayout_2->addLayout(histLayout, 2, 3, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        QWidget::setTabOrder(ledIntensityEdit, frameRateEdit);
        QWidget::setTabOrder(frameRateEdit, durationEdit);
        QWidget::setTabOrder(durationEdit, liveScanBtn);
        QWidget::setTabOrder(liveScanBtn, advancedSetupBtn);
        QWidget::setTabOrder(advancedSetupBtn, dataTypeList);
        QWidget::setTabOrder(dataTypeList, settingsBtn);
        QWidget::setTabOrder(settingsBtn, startAcquisitionBtn);
        QWidget::setTabOrder(startAcquisitionBtn, stageNavigationBtn);
        QWidget::setTabOrder(stageNavigationBtn, plateFormatDropDown);
        QWidget::setTabOrder(plateFormatDropDown, levelsSlider);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Curi Bio - Nautilai", nullptr));
        levelMax->setText(QCoreApplication::translate("MainWindow", "4095", nullptr));
        levelMin->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        startAcquisitionBtn->setText(QCoreApplication::translate("MainWindow", "Start Acquisition", nullptr));
        advancedSetupBtn->setText(QCoreApplication::translate("MainWindow", "Advanced Setup", nullptr));
        liveScanBtn->setText(QCoreApplication::translate("MainWindow", "Live Scan", nullptr));
        stageNavigationBtn->setText(QCoreApplication::translate("MainWindow", "Stage Navigation", nullptr));
        settingsBtn->setText(QCoreApplication::translate("MainWindow", "Save Directory, Format, Name, Autoupload", nullptr));
        ledIntensityLabel->setText(QCoreApplication::translate("MainWindow", "LED Intensity (%)", nullptr));
        frameRateFPSLabel->setText(QCoreApplication::translate("MainWindow", "Frame Rate (FPS)", nullptr));
        durationSLabel->setText(QCoreApplication::translate("MainWindow", "Duration (s)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
