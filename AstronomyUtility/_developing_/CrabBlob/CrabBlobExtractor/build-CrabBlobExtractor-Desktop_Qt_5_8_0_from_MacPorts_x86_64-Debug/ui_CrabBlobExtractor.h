/********************************************************************************
** Form generated from reading UI file 'CrabBlobExtractor.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CRABBLOBEXTRACTOR_H
#define UI_CRABBLOBEXTRACTOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CrabBlobExtractor
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_Header;
    QLabel *label_FitsImage;
    QLineEdit *lineEdit_FitsImage;
    QPushButton *pushButton_Open;
    QPushButton *pushButton_Blobs;
    QPushButton *pushButton_Close;
    QHBoxLayout *horizontalLayout_Body;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *CrabBlobExtractor)
    {
        if (CrabBlobExtractor->objectName().isEmpty())
            CrabBlobExtractor->setObjectName(QStringLiteral("CrabBlobExtractor"));
        CrabBlobExtractor->resize(773, 369);
        verticalLayout = new QVBoxLayout(CrabBlobExtractor);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_Header = new QHBoxLayout();
        horizontalLayout_Header->setSpacing(1);
        horizontalLayout_Header->setObjectName(QStringLiteral("horizontalLayout_Header"));
        label_FitsImage = new QLabel(CrabBlobExtractor);
        label_FitsImage->setObjectName(QStringLiteral("label_FitsImage"));

        horizontalLayout_Header->addWidget(label_FitsImage);

        lineEdit_FitsImage = new QLineEdit(CrabBlobExtractor);
        lineEdit_FitsImage->setObjectName(QStringLiteral("lineEdit_FitsImage"));

        horizontalLayout_Header->addWidget(lineEdit_FitsImage);

        pushButton_Open = new QPushButton(CrabBlobExtractor);
        pushButton_Open->setObjectName(QStringLiteral("pushButton_Open"));

        horizontalLayout_Header->addWidget(pushButton_Open);

        pushButton_Blobs = new QPushButton(CrabBlobExtractor);
        pushButton_Blobs->setObjectName(QStringLiteral("pushButton_Blobs"));

        horizontalLayout_Header->addWidget(pushButton_Blobs);

        pushButton_Close = new QPushButton(CrabBlobExtractor);
        pushButton_Close->setObjectName(QStringLiteral("pushButton_Close"));

        horizontalLayout_Header->addWidget(pushButton_Close);


        verticalLayout->addLayout(horizontalLayout_Header);

        horizontalLayout_Body = new QHBoxLayout();
        horizontalLayout_Body->setSpacing(1);
        horizontalLayout_Body->setObjectName(QStringLiteral("horizontalLayout_Body"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        horizontalLayout_Body->addItem(verticalSpacer);


        verticalLayout->addLayout(horizontalLayout_Body);


        retranslateUi(CrabBlobExtractor);

        QMetaObject::connectSlotsByName(CrabBlobExtractor);
    } // setupUi

    void retranslateUi(QWidget *CrabBlobExtractor)
    {
        CrabBlobExtractor->setWindowTitle(QApplication::translate("CrabBlobExtractor", "CrabBlobExtractor", Q_NULLPTR));
        label_FitsImage->setText(QApplication::translate("CrabBlobExtractor", "FITS image", Q_NULLPTR));
        pushButton_Open->setText(QApplication::translate("CrabBlobExtractor", "Open", Q_NULLPTR));
        pushButton_Blobs->setText(QApplication::translate("CrabBlobExtractor", "Blobs!", Q_NULLPTR));
        pushButton_Close->setText(QApplication::translate("CrabBlobExtractor", "Close", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CrabBlobExtractor: public Ui_CrabBlobExtractor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CRABBLOBEXTRACTOR_H
