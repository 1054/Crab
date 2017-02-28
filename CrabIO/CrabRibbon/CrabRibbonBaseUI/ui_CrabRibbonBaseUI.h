/********************************************************************************
** Form generated from reading UI file 'CrabRibbonBaseUI.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CRABRIBBONBASEUI_H
#define UI_CRABRIBBONBASEUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "CrabRibbonCtrlButton.h"

QT_BEGIN_NAMESPACE

class Ui_CrabRibbonBaseUI
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *Head;
    QHBoxLayout *horizontalLayout;
    QLabel *Label;
    CrabRibbonCtrlButton *ButtonGo;
    CrabRibbonCtrlButton *ButtonBye;
    QWidget *Core;
    QWidget *Foot;

    void setupUi(QWidget *CrabRibbonBaseUI)
    {
        if (CrabRibbonBaseUI->objectName().isEmpty())
            CrabRibbonBaseUI->setObjectName(QStringLiteral("CrabRibbonBaseUI"));
        CrabRibbonBaseUI->resize(600, 400);
        CrabRibbonBaseUI->setMouseTracking(true);
        CrabRibbonBaseUI->setStyleSheet(QLatin1String("QWidget#CrabRibbonBaseUI{\n"
"background-color: #67a9cf;\n"
"border: 0px solid transparent;\n"
"/*\n"
"border: 2px solid rgba(103,169,207,15%);\n"
"border-style: outset;\n"
"border-radius: 10px;\n"
"/*\n"
"border-style: outset;\n"
"border-width: 2px;\n"
"border-radius: 10px;\n"
"border-color: beige;\n"
"/*\n"
"http://qt-project.org/doc/qt-4.8/stylesheet-examples.html\n"
"*/\n"
"}\n"
"\n"
"QLabel{\n"
"color:white;\n"
"border:0px solid transparent;\n"
"}\n"
"\n"
"CrabRibbonCtrlButton{\n"
"color:white;\n"
"border:0px solid transparent;\n"
"}\n"
"\n"
"CrabRibbonCtrlButton:hover{\n"
"color:#d0d1e6;\n"
"border:0px solid transparent;\n"
"}\n"
"\n"
"CrabRibbonCtrlButton:pressed{\n"
"color:#d0d1e6;\n"
"background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #67a9cf, stop:0.5 #3690c0, stop:1 #67a9cf);\n"
"border:0px solid transparent;\n"
"}\n"
"\n"
"CrabRibbonCtrlButton::open{\n"
"color:#d0d1e6;\n"
"background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #67a9cf, stop:0.5 #3690c0, stop:1 #67a9cf);\n"
"bo"
                        "rder:0px solid transparent;\n"
"}\n"
"\n"
"CrabRibbonCtrlButton::menu-indicator{\n"
"image: none;\n"
"width:0px;height:0px;\n"
"subcontrol-origin: padding;\n"
"subcontrol-position: top right;\n"
"}\n"
""));
        verticalLayout = new QVBoxLayout(CrabRibbonBaseUI);
        verticalLayout->setSpacing(2);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(9, 2, 9, 2);
        Head = new QWidget(CrabRibbonBaseUI);
        Head->setObjectName(QStringLiteral("Head"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Head->sizePolicy().hasHeightForWidth());
        Head->setSizePolicy(sizePolicy);
        Head->setMinimumSize(QSize(0, 22));
        Head->setMaximumSize(QSize(16777215, 22));
        horizontalLayout = new QHBoxLayout(Head);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(12, 0, 0, 0);
        Label = new QLabel(Head);
        Label->setObjectName(QStringLiteral("Label"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(Label->sizePolicy().hasHeightForWidth());
        Label->setSizePolicy(sizePolicy1);
        Label->setMinimumSize(QSize(0, 20));
        Label->setMaximumSize(QSize(16777215, 20));

        horizontalLayout->addWidget(Label);

        ButtonGo = new CrabRibbonCtrlButton(Head);
        ButtonGo->setObjectName(QStringLiteral("ButtonGo"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(ButtonGo->sizePolicy().hasHeightForWidth());
        ButtonGo->setSizePolicy(sizePolicy2);
        ButtonGo->setMinimumSize(QSize(60, 20));
        ButtonGo->setMaximumSize(QSize(60, 20));
        ButtonGo->setFlat(true);

        horizontalLayout->addWidget(ButtonGo);

        ButtonBye = new CrabRibbonCtrlButton(Head);
        ButtonBye->setObjectName(QStringLiteral("ButtonBye"));
        sizePolicy2.setHeightForWidth(ButtonBye->sizePolicy().hasHeightForWidth());
        ButtonBye->setSizePolicy(sizePolicy2);
        ButtonBye->setMinimumSize(QSize(60, 20));
        ButtonBye->setMaximumSize(QSize(60, 20));
        ButtonBye->setFlat(true);

        horizontalLayout->addWidget(ButtonBye);


        verticalLayout->addWidget(Head);

        Core = new QWidget(CrabRibbonBaseUI);
        Core->setObjectName(QStringLiteral("Core"));
        Core->setAutoFillBackground(false);
        Core->setStyleSheet(QLatin1String("QWidget#Core{\n"
"background-color:white;\n"
"border: 1px solid #a6bddb;\n"
"border-radius: 5px;\n"
"}"));

        verticalLayout->addWidget(Core);

        Foot = new QWidget(CrabRibbonBaseUI);
        Foot->setObjectName(QStringLiteral("Foot"));
        sizePolicy.setHeightForWidth(Foot->sizePolicy().hasHeightForWidth());
        Foot->setSizePolicy(sizePolicy);
        Foot->setMinimumSize(QSize(0, 16));
        Foot->setMaximumSize(QSize(16777215, 16));
        Foot->setAutoFillBackground(false);
        Foot->setStyleSheet(QLatin1String("QWidget#Foot{\n"
"background-color:transparent;\n"
"/*background-color:qlineargradient(x1:0,y1:1,x2:0,y2:0,stop:0 #67a9cf,stop:1 white);*/\n"
"}"));

        verticalLayout->addWidget(Foot);


        retranslateUi(CrabRibbonBaseUI);

        QMetaObject::connectSlotsByName(CrabRibbonBaseUI);
    } // setupUi

    void retranslateUi(QWidget *CrabRibbonBaseUI)
    {
        CrabRibbonBaseUI->setWindowTitle(QApplication::translate("CrabRibbonBaseUI", "CrabRibbonBaseUI", 0));
        Label->setText(QApplication::translate("CrabRibbonBaseUI", "Welcome", 0));
        ButtonGo->setText(QApplication::translate("CrabRibbonBaseUI", "Go", 0));
        ButtonBye->setText(QApplication::translate("CrabRibbonBaseUI", "Bye", 0));
    } // retranslateUi

};

namespace Ui {
    class CrabRibbonBaseUI: public Ui_CrabRibbonBaseUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CRABRIBBONBASEUI_H
