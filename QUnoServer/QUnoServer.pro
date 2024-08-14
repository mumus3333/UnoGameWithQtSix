TEMPLATE = app
TARGET = QUnoServer

QT += core gui widgets network

# Asegúrate de que esta ruta sea correcta
INCLUDEPATH += $$PWD/../QUnoClient

HEADERS += \
    mainwindow.h \
    ../QUnoClient/mazo.h

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ../QUnoClient/mazo.cpp

CONFIG += c++17
#mod