#-------------------------------------------------
#
# Project created by QtCreator 2011-11-15T19:40:01
#
#-------------------------------------------------

QT        += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET     = fitsHeader
TEMPLATE   = app

SOURCES   += main.cpp\
             fitsHeader.cpp \
             myHighLighter.cpp \
             mySearchDialog.cpp \
             myfitsIO.cpp

HEADERS   += fitsHeader.h \
             myHighLighter.h \
             mySearchDialog.h \
             myfitsIO.h

FORMS     += fitsHeader.ui \
             mySearchDialog.ui

RESOURCES += ico.qrc
RC_FILE    = fitsHeader.rc
#<TODO> OpenFileHistory.ini
#<TODO> LastOpenedDirectory
