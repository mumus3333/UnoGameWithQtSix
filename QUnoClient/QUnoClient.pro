######################################################################
# Automatically generated by qmake (3.1) Sun Aug 11 17:21:02 2024
######################################################################

TEMPLATE = app
TARGET = QUnoClient
INCLUDEPATH += .

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += label.h mainwindow.h mazo.h
SOURCES += label.cpp main.cpp mainwindow.cpp mazo.cpp
QT += widgets opengl openglwidgets network
LIBS += -lGL -lGLU -lglut
