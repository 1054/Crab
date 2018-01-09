/********************************************************************************
** Form generated from reading UI file 'CrabWordsOnTop.ui'
**
** Created by: Qt User Interface Compiler version 5.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CRABWORDSONTOP_H
#define UI_CRABWORDSONTOP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CrabWordsOnTop
{
public:
    QVBoxLayout *verticalLayout;
    QLineEdit *lineEdit;

    void setupUi(QWidget *CrabWordsOnTop)
    {
        if (CrabWordsOnTop->objectName().isEmpty())
            CrabWordsOnTop->setObjectName(QStringLiteral("CrabWordsOnTop"));
        CrabWordsOnTop->resize(218, 25);
        verticalLayout = new QVBoxLayout(CrabWordsOnTop);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        lineEdit = new QLineEdit(CrabWordsOnTop);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setEnabled(false);
        QFont font;
        font.setFamily(QStringLiteral("FZSuXinShiLiuKaiS-R-GB"));
        font.setPointSize(13);
        lineEdit->setFont(font);
        lineEdit->setAutoFillBackground(false);
        lineEdit->setStyleSheet(QStringLiteral("color:black; background: transparent;"));
        lineEdit->setFrame(false);
        lineEdit->setReadOnly(true);

        verticalLayout->addWidget(lineEdit);


        retranslateUi(CrabWordsOnTop);

        QMetaObject::connectSlotsByName(CrabWordsOnTop);
    } // setupUi

    void retranslateUi(QWidget *CrabWordsOnTop)
    {
        CrabWordsOnTop->setWindowTitle(QApplication::translate("CrabWordsOnTop", "CrabWordsOnTop", Q_NULLPTR));
        lineEdit->setText(QApplication::translate("CrabWordsOnTop", "\346\265\267\347\272\263\347\231\276\345\267\235", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CrabWordsOnTop: public Ui_CrabWordsOnTop {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CRABWORDSONTOP_H
