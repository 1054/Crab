/********************************************************************************
** Form generated from reading UI file 'CrabNetworkSpeedMonitor.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CRABNETWORKSPEEDMONITOR_H
#define UI_CRABNETWORKSPEEDMONITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CrabNetworkSpeedMonitor
{
public:
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QLineEdit *lineEdit_1;
    QLineEdit *lineEdit_2;
    QVBoxLayout *verticalLayout_2;
    QLineEdit *lineEdit_3;
    QLineEdit *lineEdit_4;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBox;
    QLabel *label_1;
    QHBoxLayout *horizontalLayout;
    QLabel *label_os;
    QLabel *label_ms;

    void setupUi(QWidget *CrabNetworkSpeedMonitor)
    {
        if (CrabNetworkSpeedMonitor->objectName().isEmpty())
            CrabNetworkSpeedMonitor->setObjectName(QStringLiteral("CrabNetworkSpeedMonitor"));
        CrabNetworkSpeedMonitor->resize(644, 80);
        horizontalLayout_3 = new QHBoxLayout(CrabNetworkSpeedMonitor);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(1);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        lineEdit_1 = new QLineEdit(CrabNetworkSpeedMonitor);
        lineEdit_1->setObjectName(QStringLiteral("lineEdit_1"));
        lineEdit_1->setAutoFillBackground(false);
        lineEdit_1->setStyleSheet(QStringLiteral("background-color: rgba(0, 0, 0, 0); "));
        lineEdit_1->setReadOnly(true);

        verticalLayout->addWidget(lineEdit_1);

        lineEdit_2 = new QLineEdit(CrabNetworkSpeedMonitor);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setEnabled(true);
        lineEdit_2->setAutoFillBackground(false);
        lineEdit_2->setStyleSheet(QStringLiteral("background-color: rgba(0, 0, 0, 0); "));
        lineEdit_2->setFrame(true);
        lineEdit_2->setReadOnly(true);

        verticalLayout->addWidget(lineEdit_2);


        horizontalLayout_3->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(1);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        lineEdit_3 = new QLineEdit(CrabNetworkSpeedMonitor);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));
        lineEdit_3->setAutoFillBackground(false);
        lineEdit_3->setStyleSheet(QStringLiteral("background-color: rgba(0, 0, 0, 0); "));
        lineEdit_3->setFrame(true);
        lineEdit_3->setReadOnly(true);

        verticalLayout_2->addWidget(lineEdit_3);

        lineEdit_4 = new QLineEdit(CrabNetworkSpeedMonitor);
        lineEdit_4->setObjectName(QStringLiteral("lineEdit_4"));
        lineEdit_4->setEnabled(true);
        lineEdit_4->setAutoFillBackground(false);
        lineEdit_4->setStyleSheet(QStringLiteral("background-color: rgba(0, 0, 0, 0); "));
        lineEdit_4->setFrame(true);
        lineEdit_4->setReadOnly(true);

        verticalLayout_2->addWidget(lineEdit_4);


        horizontalLayout_3->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(1);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(1);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(CrabNetworkSpeedMonitor);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        doubleSpinBox = new QDoubleSpinBox(CrabNetworkSpeedMonitor);
        doubleSpinBox->setObjectName(QStringLiteral("doubleSpinBox"));
        doubleSpinBox->setMinimumSize(QSize(76, 0));
        doubleSpinBox->setMaximum(999.99);
        doubleSpinBox->setSingleStep(0.25);

        horizontalLayout_2->addWidget(doubleSpinBox);

        label_1 = new QLabel(CrabNetworkSpeedMonitor);
        label_1->setObjectName(QStringLiteral("label_1"));

        horizontalLayout_2->addWidget(label_1);


        verticalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(1);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_os = new QLabel(CrabNetworkSpeedMonitor);
        label_os->setObjectName(QStringLiteral("label_os"));

        horizontalLayout->addWidget(label_os);

        label_ms = new QLabel(CrabNetworkSpeedMonitor);
        label_ms->setObjectName(QStringLiteral("label_ms"));

        horizontalLayout->addWidget(label_ms);


        verticalLayout_3->addLayout(horizontalLayout);


        horizontalLayout_3->addLayout(verticalLayout_3);


        retranslateUi(CrabNetworkSpeedMonitor);

        QMetaObject::connectSlotsByName(CrabNetworkSpeedMonitor);
    } // setupUi

    void retranslateUi(QWidget *CrabNetworkSpeedMonitor)
    {
        CrabNetworkSpeedMonitor->setWindowTitle(QApplication::translate("CrabNetworkSpeedMonitor", "CrabNetworkSpeedMonitor", Q_NULLPTR));
        lineEdit_3->setText(QString());
        label_2->setText(QApplication::translate("CrabNetworkSpeedMonitor", "update every", Q_NULLPTR));
        label_1->setText(QApplication::translate("CrabNetworkSpeedMonitor", "sec", Q_NULLPTR));
        label_os->setText(QApplication::translate("CrabNetworkSpeedMonitor", "os", Q_NULLPTR));
        label_ms->setText(QApplication::translate("CrabNetworkSpeedMonitor", "ms", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CrabNetworkSpeedMonitor: public Ui_CrabNetworkSpeedMonitor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CRABNETWORKSPEEDMONITOR_H
