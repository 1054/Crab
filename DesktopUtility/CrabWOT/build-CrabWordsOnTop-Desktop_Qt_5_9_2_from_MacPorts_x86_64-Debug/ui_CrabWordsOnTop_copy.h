/********************************************************************************
** Form generated from reading UI file 'CrabWordsOnTop_copy.ui'
**
** Created by: Qt User Interface Compiler version 5.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CRABWORDSONTOP_COPY_H
#define UI_CRABWORDSONTOP_COPY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CrabWordsOnTop
{
public:
    QVBoxLayout *verticalLayout;
    QTextEdit *textEdit;

    void setupUi(QWidget *CrabWordsOnTop)
    {
        if (CrabWordsOnTop->objectName().isEmpty())
            CrabWordsOnTop->setObjectName(QStringLiteral("CrabWordsOnTop"));
        CrabWordsOnTop->resize(159, 48);
        verticalLayout = new QVBoxLayout(CrabWordsOnTop);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        textEdit = new QTextEdit(CrabWordsOnTop);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setEnabled(true);
        textEdit->setMinimumSize(QSize(0, 24));
        QFont font;
        font.setFamily(QStringLiteral("FZSuXinShiLiuKaiS-R-GB"));
        font.setPointSize(15);
        textEdit->setFont(font);
        textEdit->viewport()->setProperty("cursor", QVariant(QCursor(Qt::OpenHandCursor)));
        textEdit->setMouseTracking(true);
        textEdit->setAutoFillBackground(false);
        textEdit->setStyleSheet(QStringLiteral("background: transparent;"));
        textEdit->setFrameShape(QFrame::NoFrame);
        textEdit->setFrameShadow(QFrame::Plain);
        textEdit->setUndoRedoEnabled(false);
        textEdit->setReadOnly(true);

        verticalLayout->addWidget(textEdit);


        retranslateUi(CrabWordsOnTop);

        QMetaObject::connectSlotsByName(CrabWordsOnTop);
    } // setupUi

    void retranslateUi(QWidget *CrabWordsOnTop)
    {
        CrabWordsOnTop->setWindowTitle(QApplication::translate("CrabWordsOnTop", "CrabWordsOnTop", Q_NULLPTR));
        textEdit->setHtml(QApplication::translate("CrabWordsOnTop", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'FZSuXinShiLiuKaiS-R-GB'; font-size:15pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">\347\262\276\350\257\232\346\211\200\350\207\263 \351\207\221\347\237\263\344\270\272\345\274\200</p></body></html>", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CrabWordsOnTop: public Ui_CrabWordsOnTop {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CRABWORDSONTOP_COPY_H
