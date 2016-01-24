/********************************************************************************
** Form generated from reading UI file 'mySearchDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYSEARCHDIALOG_H
#define UI_MYSEARCHDIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_mySearchDialog
{
public:
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QLineEdit *lineEdit;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *caseLayout;
    QRadioButton *caseButton1;
    QRadioButton *caseButton2;
    QVBoxLayout *wordLayout;
    QRadioButton *wordButton1;
    QRadioButton *wordButton2;
    QVBoxLayout *buttonLayout;
    QPushButton *searchNextButton;
    QPushButton *searchPrevButton;
    QPushButton *cancelButton;
    QButtonGroup *caseButtonGroup;
    QButtonGroup *wordButtonGroup;

    void setupUi(QDialog *mySearchDialog)
    {
        if (mySearchDialog->objectName().isEmpty())
            mySearchDialog->setObjectName(QStringLiteral("mySearchDialog"));
        mySearchDialog->resize(420, 80);
        QFont font;
        font.setFamily(QStringLiteral("Tahoma"));
        font.setPointSize(10);
        mySearchDialog->setFont(font);
        mySearchDialog->setAutoFillBackground(true);
        mySearchDialog->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        horizontalLayout_2 = new QHBoxLayout(mySearchDialog);
        horizontalLayout_2->setSpacing(3);
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(12);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(3, 6, 3, 3);
        lineEdit = new QLineEdit(mySearchDialog);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        verticalLayout->addWidget(lineEdit);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(27, 38, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        caseLayout = new QVBoxLayout();
        caseLayout->setSpacing(0);
        caseLayout->setObjectName(QStringLiteral("caseLayout"));
        caseButton1 = new QRadioButton(mySearchDialog);
        caseButtonGroup = new QButtonGroup(mySearchDialog);
        caseButtonGroup->setObjectName(QStringLiteral("caseButtonGroup"));
        caseButtonGroup->addButton(caseButton1);
        caseButton1->setObjectName(QStringLiteral("caseButton1"));
        caseButton1->setChecked(true);

        caseLayout->addWidget(caseButton1);

        caseButton2 = new QRadioButton(mySearchDialog);
        caseButtonGroup->addButton(caseButton2);
        caseButton2->setObjectName(QStringLiteral("caseButton2"));
        caseButton2->setChecked(false);

        caseLayout->addWidget(caseButton2);


        horizontalLayout->addLayout(caseLayout);

        wordLayout = new QVBoxLayout();
        wordLayout->setSpacing(0);
        wordLayout->setObjectName(QStringLiteral("wordLayout"));
        wordButton1 = new QRadioButton(mySearchDialog);
        wordButtonGroup = new QButtonGroup(mySearchDialog);
        wordButtonGroup->setObjectName(QStringLiteral("wordButtonGroup"));
        wordButtonGroup->addButton(wordButton1);
        wordButton1->setObjectName(QStringLiteral("wordButton1"));
        wordButton1->setChecked(true);

        wordLayout->addWidget(wordButton1);

        wordButton2 = new QRadioButton(mySearchDialog);
        wordButtonGroup->addButton(wordButton2);
        wordButton2->setObjectName(QStringLiteral("wordButton2"));
        wordButton2->setChecked(false);

        wordLayout->addWidget(wordButton2);


        horizontalLayout->addLayout(wordLayout);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout);

        buttonLayout = new QVBoxLayout();
        buttonLayout->setSpacing(3);
        buttonLayout->setContentsMargins(3, 3, 3, 3);
        buttonLayout->setObjectName(QStringLiteral("buttonLayout"));
        buttonLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        searchNextButton = new QPushButton(mySearchDialog);
        searchNextButton->setObjectName(QStringLiteral("searchNextButton"));
        searchNextButton->setMinimumSize(QSize(90, 24));
        searchNextButton->setMaximumSize(QSize(90, 24));

        buttonLayout->addWidget(searchNextButton);

        searchPrevButton = new QPushButton(mySearchDialog);
        searchPrevButton->setObjectName(QStringLiteral("searchPrevButton"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(searchPrevButton->sizePolicy().hasHeightForWidth());
        searchPrevButton->setSizePolicy(sizePolicy);
        searchPrevButton->setMinimumSize(QSize(90, 24));
        searchPrevButton->setMaximumSize(QSize(90, 24));

        buttonLayout->addWidget(searchPrevButton);

        cancelButton = new QPushButton(mySearchDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));
        sizePolicy.setHeightForWidth(cancelButton->sizePolicy().hasHeightForWidth());
        cancelButton->setSizePolicy(sizePolicy);
        cancelButton->setMinimumSize(QSize(90, 24));
        cancelButton->setMaximumSize(QSize(90, 24));

        buttonLayout->addWidget(cancelButton);


        horizontalLayout_2->addLayout(buttonLayout);


        retranslateUi(mySearchDialog);

        QMetaObject::connectSlotsByName(mySearchDialog);
    } // setupUi

    void retranslateUi(QDialog *mySearchDialog)
    {
        mySearchDialog->setWindowTitle(QApplication::translate("mySearchDialog", "Dialog", 0));
        caseButton1->setText(QApplication::translate("mySearchDialog", "Case Insensitive", 0));
        caseButton2->setText(QApplication::translate("mySearchDialog", "Case Sensitive", 0));
        wordButton1->setText(QApplication::translate("mySearchDialog", "All Arbitary", 0));
        wordButton2->setText(QApplication::translate("mySearchDialog", "Whole Word Only", 0));
        searchNextButton->setText(QApplication::translate("mySearchDialog", "Find Next", 0));
        searchPrevButton->setText(QApplication::translate("mySearchDialog", "Find Previous", 0));
        cancelButton->setText(QApplication::translate("mySearchDialog", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class mySearchDialog: public Ui_mySearchDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYSEARCHDIALOG_H
