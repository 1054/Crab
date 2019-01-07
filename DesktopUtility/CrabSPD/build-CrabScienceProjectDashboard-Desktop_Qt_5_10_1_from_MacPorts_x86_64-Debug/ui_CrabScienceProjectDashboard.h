/********************************************************************************
** Form generated from reading UI file 'CrabScienceProjectDashboard.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CRABSCIENCEPROJECTDASHBOARD_H
#define UI_CRABSCIENCEPROJECTDASHBOARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QColumnView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "TaskTreeViewClass.h"

QT_BEGIN_NAMESPACE

class Ui_CrabScienceProjectDashboard
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *mainLabel;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_1;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QTabWidget *tabWidget;
    QWidget *tab_Desc;
    QVBoxLayout *verticalLayout_9;
    QVBoxLayout *verticalLayout_210;
    QLabel *label_210;
    QTextEdit *textEdit_DescTab;
    QWidget *tab_Task;
    QVBoxLayout *verticalLayout_8;
    QSplitter *splitter_300;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_310;
    QLabel *label_311;
    TaskTreeViewClass *treeView_TaskTab;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_320;
    QLabel *label_321;
    QTableView *tableView_TaskTab;
    QWidget *tab_Time;
    QVBoxLayout *verticalLayout_12;
    QSplitter *splitter_400;
    QWidget *layoutWidget2;
    QVBoxLayout *verticalLayout_420;
    QLabel *label_421;
    TaskTreeViewClass *treeView_TimeTab;
    QWidget *layoutWidget3;
    QVBoxLayout *verticalLayout_410;
    QLabel *label_411;
    QTableView *tableView_TimeTab;
    QWidget *tab_Data;
    QVBoxLayout *verticalLayout_15;
    QSplitter *splitter_500;
    QWidget *layoutWidget4;
    QVBoxLayout *verticalLayout_510;
    QLabel *label_511;
    QColumnView *columnView_DataTab;
    QWidget *layoutWidget5;
    QVBoxLayout *verticalLayout_520;
    QLabel *label_521;
    QTextEdit *textEdit_DataTab;
    QWidget *tab_Tool;
    QVBoxLayout *verticalLayout_20;
    QSplitter *splitter_600;
    QWidget *layoutWidget6;
    QVBoxLayout *verticalLayout_610;
    QLabel *label_611;
    QColumnView *columnView_ToolTab;
    QWidget *layoutWidget7;
    QVBoxLayout *verticalLayout_620;
    QLabel *label_621;
    QTextEdit *textEdit_ToolTab;
    QWidget *tab_Publ;
    QVBoxLayout *verticalLayout_21;
    QSplitter *splitter_700;
    QWidget *layoutWidget8;
    QVBoxLayout *verticalLayout_710;
    QLabel *label_711;
    QColumnView *columnView_PublTab;
    QWidget *layoutWidget9;
    QVBoxLayout *verticalLayout_720;
    QLabel *label_721;
    QTextEdit *textEdit_PublTab;

    void setupUi(QWidget *CrabScienceProjectDashboard)
    {
        if (CrabScienceProjectDashboard->objectName().isEmpty())
            CrabScienceProjectDashboard->setObjectName(QStringLiteral("CrabScienceProjectDashboard"));
        CrabScienceProjectDashboard->resize(800, 500);
        CrabScienceProjectDashboard->setMinimumSize(QSize(600, 400));
        verticalLayout = new QVBoxLayout(CrabScienceProjectDashboard);
        verticalLayout->setSpacing(2);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        mainLabel = new QLabel(CrabScienceProjectDashboard);
        mainLabel->setObjectName(QStringLiteral("mainLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mainLabel->sizePolicy().hasHeightForWidth());
        mainLabel->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QStringLiteral("IM FELL English"));
        font.setPointSize(18);
        mainLabel->setFont(font);
        mainLabel->setMargin(2);

        horizontalLayout->addWidget(mainLabel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_1 = new QPushButton(CrabScienceProjectDashboard);
        pushButton_1->setObjectName(QStringLiteral("pushButton_1"));

        horizontalLayout->addWidget(pushButton_1);

        pushButton_2 = new QPushButton(CrabScienceProjectDashboard);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        horizontalLayout->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(CrabScienceProjectDashboard);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));

        horizontalLayout->addWidget(pushButton_3);


        verticalLayout->addLayout(horizontalLayout);

        tabWidget = new QTabWidget(CrabScienceProjectDashboard);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab_Desc = new QWidget();
        tab_Desc->setObjectName(QStringLiteral("tab_Desc"));
        verticalLayout_9 = new QVBoxLayout(tab_Desc);
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setContentsMargins(11, 11, 11, 11);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        verticalLayout_210 = new QVBoxLayout();
        verticalLayout_210->setSpacing(1);
        verticalLayout_210->setObjectName(QStringLiteral("verticalLayout_210"));
        label_210 = new QLabel(tab_Desc);
        label_210->setObjectName(QStringLiteral("label_210"));

        verticalLayout_210->addWidget(label_210);

        textEdit_DescTab = new QTextEdit(tab_Desc);
        textEdit_DescTab->setObjectName(QStringLiteral("textEdit_DescTab"));

        verticalLayout_210->addWidget(textEdit_DescTab);


        verticalLayout_9->addLayout(verticalLayout_210);

        tabWidget->addTab(tab_Desc, QString());
        tab_Task = new QWidget();
        tab_Task->setObjectName(QStringLiteral("tab_Task"));
        verticalLayout_8 = new QVBoxLayout(tab_Task);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        splitter_300 = new QSplitter(tab_Task);
        splitter_300->setObjectName(QStringLiteral("splitter_300"));
        splitter_300->setOrientation(Qt::Horizontal);
        layoutWidget = new QWidget(splitter_300);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        verticalLayout_310 = new QVBoxLayout(layoutWidget);
        verticalLayout_310->setSpacing(1);
        verticalLayout_310->setContentsMargins(11, 11, 11, 11);
        verticalLayout_310->setObjectName(QStringLiteral("verticalLayout_310"));
        verticalLayout_310->setContentsMargins(0, 0, 0, 0);
        label_311 = new QLabel(layoutWidget);
        label_311->setObjectName(QStringLiteral("label_311"));

        verticalLayout_310->addWidget(label_311);

        treeView_TaskTab = new TaskTreeViewClass(layoutWidget);
        treeView_TaskTab->setObjectName(QStringLiteral("treeView_TaskTab"));

        verticalLayout_310->addWidget(treeView_TaskTab);

        splitter_300->addWidget(layoutWidget);
        layoutWidget1 = new QWidget(splitter_300);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        verticalLayout_320 = new QVBoxLayout(layoutWidget1);
        verticalLayout_320->setSpacing(1);
        verticalLayout_320->setContentsMargins(11, 11, 11, 11);
        verticalLayout_320->setObjectName(QStringLiteral("verticalLayout_320"));
        verticalLayout_320->setContentsMargins(0, 0, 0, 0);
        label_321 = new QLabel(layoutWidget1);
        label_321->setObjectName(QStringLiteral("label_321"));

        verticalLayout_320->addWidget(label_321);

        tableView_TaskTab = new QTableView(layoutWidget1);
        tableView_TaskTab->setObjectName(QStringLiteral("tableView_TaskTab"));

        verticalLayout_320->addWidget(tableView_TaskTab);

        splitter_300->addWidget(layoutWidget1);

        verticalLayout_8->addWidget(splitter_300);

        tabWidget->addTab(tab_Task, QString());
        tab_Time = new QWidget();
        tab_Time->setObjectName(QStringLiteral("tab_Time"));
        verticalLayout_12 = new QVBoxLayout(tab_Time);
        verticalLayout_12->setSpacing(6);
        verticalLayout_12->setContentsMargins(11, 11, 11, 11);
        verticalLayout_12->setObjectName(QStringLiteral("verticalLayout_12"));
        splitter_400 = new QSplitter(tab_Time);
        splitter_400->setObjectName(QStringLiteral("splitter_400"));
        splitter_400->setOrientation(Qt::Horizontal);
        layoutWidget2 = new QWidget(splitter_400);
        layoutWidget2->setObjectName(QStringLiteral("layoutWidget2"));
        verticalLayout_420 = new QVBoxLayout(layoutWidget2);
        verticalLayout_420->setSpacing(1);
        verticalLayout_420->setContentsMargins(11, 11, 11, 11);
        verticalLayout_420->setObjectName(QStringLiteral("verticalLayout_420"));
        verticalLayout_420->setContentsMargins(0, 0, 0, 0);
        label_421 = new QLabel(layoutWidget2);
        label_421->setObjectName(QStringLiteral("label_421"));

        verticalLayout_420->addWidget(label_421);

        treeView_TimeTab = new TaskTreeViewClass(layoutWidget2);
        treeView_TimeTab->setObjectName(QStringLiteral("treeView_TimeTab"));

        verticalLayout_420->addWidget(treeView_TimeTab);

        splitter_400->addWidget(layoutWidget2);
        layoutWidget3 = new QWidget(splitter_400);
        layoutWidget3->setObjectName(QStringLiteral("layoutWidget3"));
        verticalLayout_410 = new QVBoxLayout(layoutWidget3);
        verticalLayout_410->setSpacing(1);
        verticalLayout_410->setContentsMargins(11, 11, 11, 11);
        verticalLayout_410->setObjectName(QStringLiteral("verticalLayout_410"));
        verticalLayout_410->setContentsMargins(0, 0, 0, 0);
        label_411 = new QLabel(layoutWidget3);
        label_411->setObjectName(QStringLiteral("label_411"));

        verticalLayout_410->addWidget(label_411);

        tableView_TimeTab = new QTableView(layoutWidget3);
        tableView_TimeTab->setObjectName(QStringLiteral("tableView_TimeTab"));

        verticalLayout_410->addWidget(tableView_TimeTab);

        splitter_400->addWidget(layoutWidget3);

        verticalLayout_12->addWidget(splitter_400);

        tabWidget->addTab(tab_Time, QString());
        tab_Data = new QWidget();
        tab_Data->setObjectName(QStringLiteral("tab_Data"));
        verticalLayout_15 = new QVBoxLayout(tab_Data);
        verticalLayout_15->setSpacing(6);
        verticalLayout_15->setContentsMargins(11, 11, 11, 11);
        verticalLayout_15->setObjectName(QStringLiteral("verticalLayout_15"));
        splitter_500 = new QSplitter(tab_Data);
        splitter_500->setObjectName(QStringLiteral("splitter_500"));
        splitter_500->setOrientation(Qt::Horizontal);
        layoutWidget4 = new QWidget(splitter_500);
        layoutWidget4->setObjectName(QStringLiteral("layoutWidget4"));
        verticalLayout_510 = new QVBoxLayout(layoutWidget4);
        verticalLayout_510->setSpacing(1);
        verticalLayout_510->setContentsMargins(11, 11, 11, 11);
        verticalLayout_510->setObjectName(QStringLiteral("verticalLayout_510"));
        verticalLayout_510->setContentsMargins(0, 0, 0, 0);
        label_511 = new QLabel(layoutWidget4);
        label_511->setObjectName(QStringLiteral("label_511"));

        verticalLayout_510->addWidget(label_511);

        columnView_DataTab = new QColumnView(layoutWidget4);
        columnView_DataTab->setObjectName(QStringLiteral("columnView_DataTab"));

        verticalLayout_510->addWidget(columnView_DataTab);

        splitter_500->addWidget(layoutWidget4);
        layoutWidget5 = new QWidget(splitter_500);
        layoutWidget5->setObjectName(QStringLiteral("layoutWidget5"));
        verticalLayout_520 = new QVBoxLayout(layoutWidget5);
        verticalLayout_520->setSpacing(1);
        verticalLayout_520->setContentsMargins(11, 11, 11, 11);
        verticalLayout_520->setObjectName(QStringLiteral("verticalLayout_520"));
        verticalLayout_520->setContentsMargins(0, 0, 0, 0);
        label_521 = new QLabel(layoutWidget5);
        label_521->setObjectName(QStringLiteral("label_521"));

        verticalLayout_520->addWidget(label_521);

        textEdit_DataTab = new QTextEdit(layoutWidget5);
        textEdit_DataTab->setObjectName(QStringLiteral("textEdit_DataTab"));

        verticalLayout_520->addWidget(textEdit_DataTab);

        splitter_500->addWidget(layoutWidget5);

        verticalLayout_15->addWidget(splitter_500);

        tabWidget->addTab(tab_Data, QString());
        tab_Tool = new QWidget();
        tab_Tool->setObjectName(QStringLiteral("tab_Tool"));
        verticalLayout_20 = new QVBoxLayout(tab_Tool);
        verticalLayout_20->setSpacing(6);
        verticalLayout_20->setContentsMargins(11, 11, 11, 11);
        verticalLayout_20->setObjectName(QStringLiteral("verticalLayout_20"));
        splitter_600 = new QSplitter(tab_Tool);
        splitter_600->setObjectName(QStringLiteral("splitter_600"));
        splitter_600->setOrientation(Qt::Horizontal);
        layoutWidget6 = new QWidget(splitter_600);
        layoutWidget6->setObjectName(QStringLiteral("layoutWidget6"));
        verticalLayout_610 = new QVBoxLayout(layoutWidget6);
        verticalLayout_610->setSpacing(1);
        verticalLayout_610->setContentsMargins(11, 11, 11, 11);
        verticalLayout_610->setObjectName(QStringLiteral("verticalLayout_610"));
        verticalLayout_610->setContentsMargins(0, 0, 0, 0);
        label_611 = new QLabel(layoutWidget6);
        label_611->setObjectName(QStringLiteral("label_611"));

        verticalLayout_610->addWidget(label_611);

        columnView_ToolTab = new QColumnView(layoutWidget6);
        columnView_ToolTab->setObjectName(QStringLiteral("columnView_ToolTab"));

        verticalLayout_610->addWidget(columnView_ToolTab);

        splitter_600->addWidget(layoutWidget6);
        layoutWidget7 = new QWidget(splitter_600);
        layoutWidget7->setObjectName(QStringLiteral("layoutWidget7"));
        verticalLayout_620 = new QVBoxLayout(layoutWidget7);
        verticalLayout_620->setSpacing(1);
        verticalLayout_620->setContentsMargins(11, 11, 11, 11);
        verticalLayout_620->setObjectName(QStringLiteral("verticalLayout_620"));
        verticalLayout_620->setContentsMargins(0, 0, 0, 0);
        label_621 = new QLabel(layoutWidget7);
        label_621->setObjectName(QStringLiteral("label_621"));

        verticalLayout_620->addWidget(label_621);

        textEdit_ToolTab = new QTextEdit(layoutWidget7);
        textEdit_ToolTab->setObjectName(QStringLiteral("textEdit_ToolTab"));

        verticalLayout_620->addWidget(textEdit_ToolTab);

        splitter_600->addWidget(layoutWidget7);

        verticalLayout_20->addWidget(splitter_600);

        tabWidget->addTab(tab_Tool, QString());
        tab_Publ = new QWidget();
        tab_Publ->setObjectName(QStringLiteral("tab_Publ"));
        verticalLayout_21 = new QVBoxLayout(tab_Publ);
        verticalLayout_21->setSpacing(6);
        verticalLayout_21->setContentsMargins(11, 11, 11, 11);
        verticalLayout_21->setObjectName(QStringLiteral("verticalLayout_21"));
        splitter_700 = new QSplitter(tab_Publ);
        splitter_700->setObjectName(QStringLiteral("splitter_700"));
        splitter_700->setOrientation(Qt::Horizontal);
        layoutWidget8 = new QWidget(splitter_700);
        layoutWidget8->setObjectName(QStringLiteral("layoutWidget8"));
        verticalLayout_710 = new QVBoxLayout(layoutWidget8);
        verticalLayout_710->setSpacing(1);
        verticalLayout_710->setContentsMargins(11, 11, 11, 11);
        verticalLayout_710->setObjectName(QStringLiteral("verticalLayout_710"));
        verticalLayout_710->setContentsMargins(0, 0, 0, 0);
        label_711 = new QLabel(layoutWidget8);
        label_711->setObjectName(QStringLiteral("label_711"));

        verticalLayout_710->addWidget(label_711);

        columnView_PublTab = new QColumnView(layoutWidget8);
        columnView_PublTab->setObjectName(QStringLiteral("columnView_PublTab"));

        verticalLayout_710->addWidget(columnView_PublTab);

        splitter_700->addWidget(layoutWidget8);
        layoutWidget9 = new QWidget(splitter_700);
        layoutWidget9->setObjectName(QStringLiteral("layoutWidget9"));
        verticalLayout_720 = new QVBoxLayout(layoutWidget9);
        verticalLayout_720->setSpacing(1);
        verticalLayout_720->setContentsMargins(11, 11, 11, 11);
        verticalLayout_720->setObjectName(QStringLiteral("verticalLayout_720"));
        verticalLayout_720->setContentsMargins(0, 0, 0, 0);
        label_721 = new QLabel(layoutWidget9);
        label_721->setObjectName(QStringLiteral("label_721"));

        verticalLayout_720->addWidget(label_721);

        textEdit_PublTab = new QTextEdit(layoutWidget9);
        textEdit_PublTab->setObjectName(QStringLiteral("textEdit_PublTab"));

        verticalLayout_720->addWidget(textEdit_PublTab);

        splitter_700->addWidget(layoutWidget9);

        verticalLayout_21->addWidget(splitter_700);

        tabWidget->addTab(tab_Publ, QString());

        verticalLayout->addWidget(tabWidget);


        retranslateUi(CrabScienceProjectDashboard);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(CrabScienceProjectDashboard);
    } // setupUi

    void retranslateUi(QWidget *CrabScienceProjectDashboard)
    {
        CrabScienceProjectDashboard->setWindowTitle(QApplication::translate("CrabScienceProjectDashboard", "CrabScienceProjectDashboard", nullptr));
        mainLabel->setText(QApplication::translate("CrabScienceProjectDashboard", "My Project (double click to edit)", nullptr));
        pushButton_1->setText(QApplication::translate("CrabScienceProjectDashboard", "B1", nullptr));
        pushButton_2->setText(QApplication::translate("CrabScienceProjectDashboard", "B2", nullptr));
        pushButton_3->setText(QApplication::translate("CrabScienceProjectDashboard", "B3", nullptr));
        label_210->setText(QApplication::translate("CrabScienceProjectDashboard", "Project Description", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_Desc), QApplication::translate("CrabScienceProjectDashboard", "Desc", nullptr));
        label_311->setText(QApplication::translate("CrabScienceProjectDashboard", "List of Tasks", nullptr));
        label_321->setText(QApplication::translate("CrabScienceProjectDashboard", "Details of the Selected Task", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_Task), QApplication::translate("CrabScienceProjectDashboard", "Task", nullptr));
        label_421->setText(QApplication::translate("CrabScienceProjectDashboard", "List of Tasks", nullptr));
        label_411->setText(QApplication::translate("CrabScienceProjectDashboard", "Time Table", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_Time), QApplication::translate("CrabScienceProjectDashboard", "Time", nullptr));
        label_511->setText(QApplication::translate("CrabScienceProjectDashboard", "File Browser", nullptr));
        label_521->setText(QApplication::translate("CrabScienceProjectDashboard", "ReadMe Note", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_Data), QApplication::translate("CrabScienceProjectDashboard", "Data", nullptr));
        label_611->setText(QApplication::translate("CrabScienceProjectDashboard", "File Browser", nullptr));
        label_621->setText(QApplication::translate("CrabScienceProjectDashboard", "ReadMe Note", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_Tool), QApplication::translate("CrabScienceProjectDashboard", "Tool", nullptr));
        label_711->setText(QApplication::translate("CrabScienceProjectDashboard", "File Browser", nullptr));
        label_721->setText(QApplication::translate("CrabScienceProjectDashboard", "ReadMe Note", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_Publ), QApplication::translate("CrabScienceProjectDashboard", "Publ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CrabScienceProjectDashboard: public Ui_CrabScienceProjectDashboard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CRABSCIENCEPROJECTDASHBOARD_H
