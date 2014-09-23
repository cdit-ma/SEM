#-------------------------------------------------
#
# Project created by QtCreator 2014-06-19T14:12:59
#
#-------------------------------------------------

QT       += core
QT       += xml
QT       += gui
QT       += widgets

TARGET = MEDEA
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += \
    Controller/graphmlcontroller.h \
    GUI/attributetablemodel.h \
    GUI/nodeconnection.h \
    GUI/nodeitem.h \
    GUI/nodeitemtreeitem.h \
    GUI/nodeview.h \
    GUI/nodeviewtreemodel.h \
    Model/Assembly/componentassembly.h \
    Model/Assembly/componentinstance.h \
    Model/Assembly/eventport.h \
    Model/Assembly/hardwarecluster.h \
    Model/Assembly/hardwarenode.h \
    Model/Assembly/inputeventport.h \
    Model/Assembly/outputeventport.h \
    Model/attribute.h \
    Model/edge.h \
    Model/graph.h \
    Model/graphml.h \
    Model/graphmlcontainer.h \
    Model/graphmldata.h \
    Model/graphmlkey.h \
    Model/model.h \
    Model/node.h \
    mainwindow.h \
    qlogger.h

SOURCES += \
    Controller/graphmlcontroller.cpp \
    GUI/attributetablemodel.cpp \
    GUI/nodeconnection.cpp \
    GUI/nodeitem.cpp \
    GUI/nodeitemtreeitem.cpp \
    GUI/nodeview.cpp \
    GUI/nodeviewtreemodel.cpp \
    Model/Assembly/componentassembly.cpp \
    Model/Assembly/componentinstance.cpp \
    Model/Assembly/eventport.cpp \
    Model/Assembly/hardwarecluster.cpp \
    Model/Assembly/hardwarenode.cpp \
    Model/Assembly/inputeventport.cpp \
    Model/Assembly/outputeventport.cpp \
    Model/attribute.cpp \
    Model/edge.cpp \
    Model/graph.cpp \
    Model/graphml.cpp \
    Model/graphmlcontainer.cpp \
    Model/graphmldata.cpp \
    Model/graphmlkey.cpp \
    Model/model.cpp \
    Model/node.cpp \
    main.cpp \
    mainwindow.cpp \
    qlogger.cpp

FORMS += \
    mainwindow.ui
