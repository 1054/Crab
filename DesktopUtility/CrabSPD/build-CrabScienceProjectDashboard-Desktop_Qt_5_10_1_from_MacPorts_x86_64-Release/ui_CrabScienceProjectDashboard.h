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
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

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
    QWidget *tab;
    QVBoxLayout *verticalLayout_5;
    QSplitter *splitter_2;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_1;
    QLabel *label_Navigator_and_Logger;
    QSplitter *splitter;
    QListView *Navigator;
    QTableView *Logger;
    QWidget *layoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_Finder;
    QColumnView *Finder;
    QWidget *layoutWidget_3;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_Writer;
    QTextEdit *Writer;
    QWidget *tab_Project_Description;
    QVBoxLayout *verticalLayout_9;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_3;
    QTextEdit *textEdit_Project_Description;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_8;
    QSplitter *splitter_3;
    QWidget *widget;
    QVBoxLayout *verticalLayout_7;
    QLabel *label;
    QTreeView *treeView;
    QWidget *widget1;
    QVBoxLayout *verticalLayout_6;
    QLabel *label_2;
    QTableView *tableView;

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
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        verticalLayout_5 = new QVBoxLayout(tab);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        splitter_2 = new QSplitter(tab);
        splitter_2->setObjectName(QStringLiteral("splitter_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(splitter_2->sizePolicy().hasHeightForWidth());
        splitter_2->setSizePolicy(sizePolicy1);
        splitter_2->setOrientation(Qt::Horizontal);
        layoutWidget = new QWidget(splitter_2);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        verticalLayout_1 = new QVBoxLayout(layoutWidget);
        verticalLayout_1->setSpacing(1);
        verticalLayout_1->setContentsMargins(11, 11, 11, 11);
        verticalLayout_1->setObjectName(QStringLiteral("verticalLayout_1"));
        verticalLayout_1->setContentsMargins(0, 0, 0, 0);
        label_Navigator_and_Logger = new QLabel(layoutWidget);
        label_Navigator_and_Logger->setObjectName(QStringLiteral("label_Navigator_and_Logger"));

        verticalLayout_1->addWidget(label_Navigator_and_Logger);

        splitter = new QSplitter(layoutWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        Navigator = new QListView(splitter);
        Navigator->setObjectName(QStringLiteral("Navigator"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(Navigator->sizePolicy().hasHeightForWidth());
        Navigator->setSizePolicy(sizePolicy2);
        splitter->addWidget(Navigator);
        Logger = new QTableView(splitter);
        Logger->setObjectName(QStringLiteral("Logger"));
        sizePolicy2.setHeightForWidth(Logger->sizePolicy().hasHeightForWidth());
        Logger->setSizePolicy(sizePolicy2);
        splitter->addWidget(Logger);

        verticalLayout_1->addWidget(splitter);

        splitter_2->addWidget(layoutWidget);
        layoutWidget_2 = new QWidget(splitter_2);
        layoutWidget_2->setObjectName(QStringLiteral("layoutWidget_2"));
        verticalLayout_2 = new QVBoxLayout(layoutWidget_2);
        verticalLayout_2->setSpacing(2);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_Finder = new QLabel(layoutWidget_2);
        label_Finder->setObjectName(QStringLiteral("label_Finder"));

        verticalLayout_2->addWidget(label_Finder);

        Finder = new QColumnView(layoutWidget_2);
        Finder->setObjectName(QStringLiteral("Finder"));
        sizePolicy2.setHeightForWidth(Finder->sizePolicy().hasHeightForWidth());
        Finder->setSizePolicy(sizePolicy2);

        verticalLayout_2->addWidget(Finder);

        splitter_2->addWidget(layoutWidget_2);
        layoutWidget_3 = new QWidget(splitter_2);
        layoutWidget_3->setObjectName(QStringLiteral("layoutWidget_3"));
        verticalLayout_3 = new QVBoxLayout(layoutWidget_3);
        verticalLayout_3->setSpacing(2);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_Writer = new QLabel(layoutWidget_3);
        label_Writer->setObjectName(QStringLiteral("label_Writer"));

        verticalLayout_3->addWidget(label_Writer);

        Writer = new QTextEdit(layoutWidget_3);
        Writer->setObjectName(QStringLiteral("Writer"));
        sizePolicy2.setHeightForWidth(Writer->sizePolicy().hasHeightForWidth());
        Writer->setSizePolicy(sizePolicy2);

        verticalLayout_3->addWidget(Writer);

        splitter_2->addWidget(layoutWidget_3);

        verticalLayout_5->addWidget(splitter_2);

        tabWidget->addTab(tab, QString());
        tab_Project_Description = new QWidget();
        tab_Project_Description->setObjectName(QStringLiteral("tab_Project_Description"));
        verticalLayout_9 = new QVBoxLayout(tab_Project_Description);
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setContentsMargins(11, 11, 11, 11);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(1);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        label_3 = new QLabel(tab_Project_Description);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout_4->addWidget(label_3);

        textEdit_Project_Description = new QTextEdit(tab_Project_Description);
        textEdit_Project_Description->setObjectName(QStringLiteral("textEdit_Project_Description"));

        verticalLayout_4->addWidget(textEdit_Project_Description);


        verticalLayout_9->addLayout(verticalLayout_4);

        tabWidget->addTab(tab_Project_Description, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        verticalLayout_8 = new QVBoxLayout(tab_2);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        splitter_3 = new QSplitter(tab_2);
        splitter_3->setObjectName(QStringLiteral("splitter_3"));
        splitter_3->setOrientation(Qt::Horizontal);
        widget = new QWidget(splitter_3);
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout_7 = new QVBoxLayout(widget);
        verticalLayout_7->setSpacing(1);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_7->addWidget(label);

        treeView = new QTreeView(widget);
        treeView->setObjectName(QStringLiteral("treeView"));

        verticalLayout_7->addWidget(treeView);

        splitter_3->addWidget(widget);
        widget1 = new QWidget(splitter_3);
        widget1->setObjectName(QStringLiteral("widget1"));
        verticalLayout_6 = new QVBoxLayout(widget1);
        verticalLayout_6->setSpacing(1);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(widget1);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout_6->addWidget(label_2);

        tableView = new QTableView(widget1);
        tableView->setObjectName(QStringLiteral("tableView"));

        verticalLayout_6->addWidget(tableView);

        splitter_3->addWidget(widget1);

        verticalLayout_8->addWidget(splitter_3);

        tabWidget->addTab(tab_2, QString());

        verticalLayout->addWidget(tabWidget);


        retranslateUi(CrabScienceProjectDashboard);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(CrabScienceProjectDashboard);
    } // setupUi

    void retranslateUi(QWidget *CrabScienceProjectDashboard)
    {
        CrabScienceProjectDashboard->setWindowTitle(QApplication::translate("CrabScienceProjectDashboard", "CrabScienceProjectDashboard", nullptr));
        mainLabel->setText(QApplication::translate("CrabScienceProjectDashboard", "double click to edit", nullptr));
        pushButton_1->setText(QApplication::translate("CrabScienceProjectDashboard", "B1", nullptr));
        pushButton_2->setText(QApplication::translate("CrabScienceProjectDashboard", "B2", nullptr));
        pushButton_3->setText(QApplication::translate("CrabScienceProjectDashboard", "B3", nullptr));
        label_Navigator_and_Logger->setText(QApplication::translate("CrabScienceProjectDashboard", "Navigator and Logger", nullptr));
        label_Finder->setText(QApplication::translate("CrabScienceProjectDashboard", "Finder", nullptr));
        label_Writer->setText(QApplication::translate("CrabScienceProjectDashboard", "Writer", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("CrabScienceProjectDashboard", "Misc", nullptr));
        label_3->setText(QApplication::translate("CrabScienceProjectDashboard", "Project Description", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_Project_Description), QApplication::translate("CrabScienceProjectDashboard", "Desc", nullptr));
        label->setText(QApplication::translate("CrabScienceProjectDashboard", "List of Tasks", nullptr));
        label_2->setText(QApplication::translate("CrabScienceProjectDashboard", "Details of the Selected Task", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("CrabScienceProjectDashboard", "Task", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CrabScienceProjectDashboard: public Ui_CrabScienceProjectDashboard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CRABSCIENCEPROJECTDASHBOARD_H
