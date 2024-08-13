TEMPLATE = app
TARGET = QUnoClient
INCLUDEPATH += .

QT += core gui widgets network

HEADERS += label.h mainwindow.h mazo.h
SOURCES += label.cpp main.cpp mainwindow.cpp mazo.cpp

CONFIG += c++17
