#-------------------------------------------------
#
# Project created by QtCreator 2011-11-17T09:52:17
#
#-------------------------------------------------

QT       += core gui widgets

TARGET    = fitsImager

TEMPLATE  = app

CONFIG   += release

INCLUDEPATH += /usr/include/qt5

SOURCES  += main.cpp\
            CrabFitsIO.cpp\
            myfitsImage.cpp \
            fitsImager.cpp \
            fitsImagePainter.cpp

HEADERS  += CrabFitsIO.h \
            myfitsImage.h \
            fitsImager.h \
            fitsImagePainter.h

FORMS    += fitsImager.ui

RESOURCES += ico.qrc

#<TODO> RA Dec display
