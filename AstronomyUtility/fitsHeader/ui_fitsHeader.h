/********************************************************************************
** Form generated from reading UI file 'fitsHeader.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FITSHEADER_H
#define UI_FITSHEADER_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_fitsHeader
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QPushButton *openButton;
    QPushButton *searchButton;
    QPushButton *closeButton;
    QTextEdit *textEdit;

    void setupUi(QWidget *fitsHeader)
    {
        if (fitsHeader->objectName().isEmpty())
            fitsHeader->setObjectName(QStringLiteral("fitsHeader"));
        fitsHeader->resize(680, 480);
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(10);
        fitsHeader->setFont(font);
        fitsHeader->setAcceptDrops(true);
        fitsHeader->setAutoFillBackground(true);
        fitsHeader->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout = new QVBoxLayout(fitsHeader);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(6, 2, 6, 2);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(fitsHeader);
        label->setObjectName(QStringLiteral("label"));
        QFont font1;
        font1.setFamily(QStringLiteral("Courier New"));
        font1.setPointSize(13);
        label->setFont(font1);

        horizontalLayout->addWidget(label);

        openButton = new QPushButton(fitsHeader);
        openButton->setObjectName(QStringLiteral("openButton"));
        openButton->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(openButton->sizePolicy().hasHeightForWidth());
        openButton->setSizePolicy(sizePolicy);
        openButton->setMaximumSize(QSize(32, 28));
        openButton->setAutoFillBackground(true);
        openButton->setStyleSheet(QStringLiteral(""));
        QIcon icon;
        icon.addFile(QStringLiteral(":/ico/5765.ico"), QSize(), QIcon::Normal, QIcon::Off);
        openButton->setIcon(icon);
        openButton->setIconSize(QSize(32, 28));
        openButton->setFlat(true);

        horizontalLayout->addWidget(openButton);

        searchButton = new QPushButton(fitsHeader);
        searchButton->setObjectName(QStringLiteral("searchButton"));
        searchButton->setMaximumSize(QSize(32, 28));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/ico/5762.ico"), QSize(), QIcon::Normal, QIcon::Off);
        searchButton->setIcon(icon1);
        searchButton->setIconSize(QSize(32, 28));
        searchButton->setFlat(true);

        horizontalLayout->addWidget(searchButton);

        closeButton = new QPushButton(fitsHeader);
        closeButton->setObjectName(QStringLiteral("closeButton"));
        closeButton->setEnabled(true);
        sizePolicy.setHeightForWidth(closeButton->sizePolicy().hasHeightForWidth());
        closeButton->setSizePolicy(sizePolicy);
        closeButton->setMaximumSize(QSize(32, 28));
        closeButton->setAutoFillBackground(true);
        closeButton->setStyleSheet(QStringLiteral(""));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/ico/5761.ico"), QSize(), QIcon::Normal, QIcon::Off);
        closeButton->setIcon(icon2);
        closeButton->setIconSize(QSize(32, 28));
        closeButton->setFlat(true);

        horizontalLayout->addWidget(closeButton);


        verticalLayout->addLayout(horizontalLayout);

        textEdit = new QTextEdit(fitsHeader);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        QFont font2;
        font2.setFamily(QStringLiteral("Courier New"));
        textEdit->setFont(font2);
        textEdit->setAutoFillBackground(true);
        textEdit->setLineWidth(80);
        textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        textEdit->setLineWrapMode(QTextEdit::FixedColumnWidth);
        textEdit->setLineWrapColumnOrWidth(80);
        textEdit->setReadOnly(true);
        textEdit->setTextInteractionFlags(Qt::TextSelectableByMouse);

        verticalLayout->addWidget(textEdit);


        retranslateUi(fitsHeader);

        QMetaObject::connectSlotsByName(fitsHeader);
    } // setupUi

    void retranslateUi(QWidget *fitsHeader)
    {
        fitsHeader->setWindowTitle(QApplication::translate("fitsHeader", "fitsHeader", 0));
#ifndef QT_NO_TOOLTIP
        fitsHeader->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("fitsHeader", "fits header", 0));
#ifndef QT_NO_TOOLTIP
        openButton->setToolTip(QApplication::translate("fitsHeader", "Open FITS", 0));
#endif // QT_NO_TOOLTIP
        openButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        searchButton->setToolTip(QApplication::translate("fitsHeader", "Search Text", 0));
#endif // QT_NO_TOOLTIP
        searchButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        closeButton->setToolTip(QApplication::translate("fitsHeader", "Close", 0));
#endif // QT_NO_TOOLTIP
        closeButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class fitsHeader: public Ui_fitsHeader {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FITSHEADER_H
