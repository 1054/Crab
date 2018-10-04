/********************************************************************************
** Form generated from reading UI file 'CrabScienceProjectDashboard.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CRABSCIENCEPROJECTDASHBOARD_H
#define UI_CRABSCIENCEPROJECTDASHBOARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QColumnView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>
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
    QSplitter *splitter_2;
    QWidget *widget;
    QVBoxLayout *verticalLayout_1;
    QLabel *label_Navigator_and_Logger;
    QSplitter *splitter;
    QListView *Navigator;
    QTableView *Logger;
    QWidget *widget1;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_Finder;
    QColumnView *Finder;
    QWidget *widget2;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_Writer;
    QTextEdit *Writer;

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

        splitter_2 = new QSplitter(CrabScienceProjectDashboard);
        splitter_2->setObjectName(QStringLiteral("splitter_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(splitter_2->sizePolicy().hasHeightForWidth());
        splitter_2->setSizePolicy(sizePolicy1);
        splitter_2->setOrientation(Qt::Horizontal);
        widget = new QWidget(splitter_2);
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout_1 = new QVBoxLayout(widget);
        verticalLayout_1->setSpacing(1);
        verticalLayout_1->setContentsMargins(11, 11, 11, 11);
        verticalLayout_1->setObjectName(QStringLiteral("verticalLayout_1"));
        verticalLayout_1->setContentsMargins(0, 0, 0, 0);
        label_Navigator_and_Logger = new QLabel(widget);
        label_Navigator_and_Logger->setObjectName(QStringLiteral("label_Navigator_and_Logger"));

        verticalLayout_1->addWidget(label_Navigator_and_Logger);

        splitter = new QSplitter(widget);
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

        splitter_2->addWidget(widget);
        widget1 = new QWidget(splitter_2);
        widget1->setObjectName(QStringLiteral("widget1"));
        verticalLayout_2 = new QVBoxLayout(widget1);
        verticalLayout_2->setSpacing(2);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_Finder = new QLabel(widget1);
        label_Finder->setObjectName(QStringLiteral("label_Finder"));

        verticalLayout_2->addWidget(label_Finder);

        Finder = new QColumnView(widget1);
        Finder->setObjectName(QStringLiteral("Finder"));
        sizePolicy2.setHeightForWidth(Finder->sizePolicy().hasHeightForWidth());
        Finder->setSizePolicy(sizePolicy2);

        verticalLayout_2->addWidget(Finder);

        splitter_2->addWidget(widget1);
        widget2 = new QWidget(splitter_2);
        widget2->setObjectName(QStringLiteral("widget2"));
        verticalLayout_3 = new QVBoxLayout(widget2);
        verticalLayout_3->setSpacing(2);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_Writer = new QLabel(widget2);
        label_Writer->setObjectName(QStringLiteral("label_Writer"));

        verticalLayout_3->addWidget(label_Writer);

        Writer = new QTextEdit(widget2);
        Writer->setObjectName(QStringLiteral("Writer"));
        sizePolicy2.setHeightForWidth(Writer->sizePolicy().hasHeightForWidth());
        Writer->setSizePolicy(sizePolicy2);

        verticalLayout_3->addWidget(Writer);

        splitter_2->addWidget(widget2);

        verticalLayout->addWidget(splitter_2);


        retranslateUi(CrabScienceProjectDashboard);

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
    } // retranslateUi

};

namespace Ui {
    class CrabScienceProjectDashboard: public Ui_CrabScienceProjectDashboard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CRABSCIENCEPROJECTDASHBOARD_H
