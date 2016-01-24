/********************************************************************************
** Form generated from reading UI file 'specViewer.ui'
**
** Created: Fri Apr 27 20:35:39 2012
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPECVIEWER_H
#define UI_SPECVIEWER_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "specPainter.h"

QT_BEGIN_NAMESPACE

class Ui_specViewer
{
public:
    QVBoxLayout *verticalLayout;
    QSpacerItem *vSpacer_top;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *hSpacer_left;
    specPainter *widget;
    QSpacerItem *hSpacer_right;
    QSpacerItem *vSpacer_bottom;

    void setupUi(QWidget *specViewer)
    {
        if (specViewer->objectName().isEmpty())
            specViewer->setObjectName(QString::fromUtf8("specViewer"));
        specViewer->resize(600, 400);
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush);
        specViewer->setPalette(palette);
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu Sans"));
        font.setPointSize(10);
        specViewer->setFont(font);
        specViewer->setMouseTracking(true);
        specViewer->setAcceptDrops(true);
        specViewer->setAutoFillBackground(true);
        specViewer->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout = new QVBoxLayout(specViewer);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        vSpacer_top = new QSpacerItem(20, 46, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(vSpacer_top);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        hSpacer_left = new QSpacerItem(36, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(hSpacer_left);

        widget = new specPainter(specViewer);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMouseTracking(true);
        widget->setAcceptDrops(true);
        widget->setAutoFillBackground(true);

        horizontalLayout->addWidget(widget);

        hSpacer_right = new QSpacerItem(36, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(hSpacer_right);


        verticalLayout->addLayout(horizontalLayout);

        vSpacer_bottom = new QSpacerItem(20, 36, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(vSpacer_bottom);


        retranslateUi(specViewer);

        QMetaObject::connectSlotsByName(specViewer);
    } // setupUi

    void retranslateUi(QWidget *specViewer)
    {
        specViewer->setWindowTitle(QApplication::translate("specViewer", "specViewer", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class specViewer: public Ui_specViewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPECVIEWER_H
