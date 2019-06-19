#-------------------------------------------------
#
# Project created by QtCreator 2017-09-06T00:03:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CrabBlobExtractor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        CrabBlobExtractor.cpp \
    CrabMe.cpp \
    CrabFitsIO.cpp \
    CrabFitsImageWidget.cpp \
    CrabFitsImageUtilities.cpp \
    CrabBlobListWidget.cpp \
    CrabBlobItem.cpp \
    3rd/blitzwave/Wavelet.cxx \
    3rd/blitzwave/WaveletDecomp.cxx

HEADERS += \
        CrabBlobExtractor.h \
    CrabMe.h \
    CrabFitsIO.h \
    CrabFitsImageWidget.h \
    CrabFitsImageUtilities.h \
    CrabBlobListWidget.h \
    CrabBlobItem.h \
    3rd/blitzwave/Wavelet.h \
    3rd/blitzwave/WaveletDecomp.h

FORMS += \
        CrabBlobExtractor.ui

INCLUDEPATH += 3rd
