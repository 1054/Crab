#-------------------------------------------------
#
# Project created by QtCreator 2011-11-18T10:28:17
#
#-------------------------------------------------

QT      += core gui

TARGET   = specViewer
TEMPLATE = app

LIBS    += -lfitscrab -L.

SOURCES += main.cpp\
           specViewer.cpp \
           specPainter.cpp \
           specAxis.cpp

HEADERS += specViewer.h \
           specPainter.h \
           specAxis.h \
           myfitsIO.h

FORMS   += specViewer.ui
