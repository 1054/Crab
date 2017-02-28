/********************************************************************************
** Form generated from reading UI file 'fitsImager.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FITSIMAGER_H
#define UI_FITSIMAGER_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "fitsImagePainter.h"

QT_BEGIN_NAMESPACE

class Ui_fitsImager
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QPushButton *openButton;
    QPushButton *saveButton;
    QPushButton *FFTButton;
    QPushButton *closeButton;
    fitsImagePainter *widget;

    void setupUi(QWidget *fitsImager)
    {
        if (fitsImager->objectName().isEmpty())
            fitsImager->setObjectName(QString::fromUtf8("fitsImager"));
        fitsImager->resize(800, 600);
        QPalette palette;
        QBrush brush(QColor(255, 250, 174, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(0, 0, 0, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(255, 255, 194, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Text, brush2);
        QBrush brush3(QColor(255, 255, 255, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush3);
        palette.setBrush(QPalette::Active, QPalette::Base, brush3);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        QBrush brush4(QColor(0, 170, 0, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush4);
        QBrush brush5(QColor(255, 255, 178, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        QBrush brush6(QColor(255, 253, 188, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush6);
        QBrush brush7(QColor(120, 120, 120, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush7);
        QBrush brush8(QColor(240, 240, 240, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush8);
        QBrush brush9(QColor(79, 79, 79, 255));
        brush9.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush9);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush4);
        fitsImager->setPalette(palette);
        QFont font;
        font.setFamily(QString::fromUtf8("Courier New"));
        font.setPointSize(10);
        fitsImager->setFont(font);
        fitsImager->setMouseTracking(true);
        fitsImager->setAcceptDrops(true);
        fitsImager->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout = new QVBoxLayout(fitsImager);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        label = new QLabel(fitsImager);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font1;
        font1.setPointSize(10);
        label->setFont(font1);
        label->setAutoFillBackground(false);
        label->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout->addWidget(label);

        openButton = new QPushButton(fitsImager);
        openButton->setObjectName(QString::fromUtf8("openButton"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(openButton->sizePolicy().hasHeightForWidth());
        openButton->setSizePolicy(sizePolicy);
        openButton->setMaximumSize(QSize(32, 28));
        openButton->setAutoFillBackground(false);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ico/02.ico"), QSize(), QIcon::Normal, QIcon::Off);
        openButton->setIcon(icon);
        openButton->setIconSize(QSize(32, 28));
        openButton->setFlat(true);

        horizontalLayout->addWidget(openButton);

        saveButton = new QPushButton(fitsImager);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));
        sizePolicy.setHeightForWidth(saveButton->sizePolicy().hasHeightForWidth());
        saveButton->setSizePolicy(sizePolicy);
        saveButton->setMinimumSize(QSize(32, 28));
        saveButton->setMaximumSize(QSize(32, 28));
        saveButton->setAutoFillBackground(false);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/ico/03.ico"), QSize(), QIcon::Normal, QIcon::Off);
        saveButton->setIcon(icon1);
        saveButton->setIconSize(QSize(32, 28));
        saveButton->setFlat(true);

        horizontalLayout->addWidget(saveButton);

        FFTButton = new QPushButton(fitsImager);
        FFTButton->setObjectName(QString::fromUtf8("FFTButton"));
        sizePolicy.setHeightForWidth(FFTButton->sizePolicy().hasHeightForWidth());
        FFTButton->setSizePolicy(sizePolicy);
        FFTButton->setMaximumSize(QSize(32, 28));
        FFTButton->setAutoFillBackground(false);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/ico/04.ico"), QSize(), QIcon::Normal, QIcon::Off);
        FFTButton->setIcon(icon2);
        FFTButton->setIconSize(QSize(32, 28));
        FFTButton->setFlat(true);

        horizontalLayout->addWidget(FFTButton);

        closeButton = new QPushButton(fitsImager);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));
        sizePolicy.setHeightForWidth(closeButton->sizePolicy().hasHeightForWidth());
        closeButton->setSizePolicy(sizePolicy);
        closeButton->setMaximumSize(QSize(32, 28));
        closeButton->setAutoFillBackground(false);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/ico/01.ico"), QSize(), QIcon::Normal, QIcon::Off);
        closeButton->setIcon(icon3);
        closeButton->setIconSize(QSize(32, 28));
        closeButton->setFlat(true);

        horizontalLayout->addWidget(closeButton);


        verticalLayout->addLayout(horizontalLayout);

        widget = new fitsImagePainter(fitsImager);
        widget->setObjectName(QString::fromUtf8("widget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        widget->setAutoFillBackground(true);

        verticalLayout->addWidget(widget);


        retranslateUi(fitsImager);

        QMetaObject::connectSlotsByName(fitsImager);
    } // setupUi

    void retranslateUi(QWidget *fitsImager)
    {
        fitsImager->setWindowTitle(QApplication::translate("fitsImager", "fitsImager", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("fitsImager", "fits imager", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        openButton->setToolTip(QApplication::translate("fitsImager", "Open", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        openButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        saveButton->setToolTip(QApplication::translate("fitsImager", "Save", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        saveButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        FFTButton->setToolTip(QApplication::translate("fitsImager", "FourierTransform", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        FFTButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        closeButton->setToolTip(QApplication::translate("fitsImager", "Close", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        closeButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class fitsImager: public Ui_fitsImager {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FITSIMAGER_H
