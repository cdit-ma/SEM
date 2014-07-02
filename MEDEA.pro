#-------------------------------------------------
#
# Project created by QtCreator 2014-06-19T14:12:59
#
#-------------------------------------------------

QT       += core
QT	+= xml

QT       -= gui

TARGET = MEDEA
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    Model/edge.cpp \
    Model/node.cpp \
    Model/graphml.cpp \
    Model/graph.cpp \
    Model/hardwarenode.cpp \
    Model/edge.cpp \
    Model/graph.cpp \
    Model/graphml.cpp \
    Model/hardwarenode.cpp \
    Model/node.cpp \
    Model/eventport.cpp \
    Model/inputeventport.cpp \
    Model/outputeventport.cpp \
    Controller/graphmlcontroller.cpp \
    Model/model.cpp \
    Model/graphmldata.cpp \
    Model/graphmlcontainer.cpp \
    Model/componentinstance.cpp \
    Model/componentassembly.cpp \
    Model/attribute.cpp \
    Model/graphmlkey.cpp

HEADERS += \
    Model/graphml.h \
    Model/edge.h \
    Model/node.h \
    Model/graph.h \
    Model/hardwarenode.h \
    Model/edge.h \
    Model/graph.h \
    Model/graphml.h \
    Model/hardwarenode.h \
    Model/node.h \
    Model/componentinstance.h \
    Model/eventport.h \
    Model/inputeventport.h \
    Model/outputeventport.h \
    Controller/graphmlcontroller.h \
    Model/model.h \
    Model/graphmldata.h \
    Model/graphmlcontainer.h \
    Model/componentassembly.h \
    Model/attribute.h \
    Model/graphmlkey.h

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
