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
    GUI/nodeview.h \
    Model/Assembly/componentassembly.h \
    Model/Assembly/componentinstance.h \
    Model/Assembly/eventport.h \
    Model/Assembly/hardwarecluster.h \
    Model/Assembly/hardwarenode.h \
    Model/edge.h \
    Model/graph.h \
    Model/graphml.h \
    Model/graphmlcontainer.h \
    Model/graphmldata.h \
    Model/graphmlkey.h \
    Model/model.h \
    Model/node.h \
    mainwindow.h \
    qlogger.h \
    Model/Assembly/attribute.h \
    Model/Workload/periodicevent.h \
    Model/Workload/component.h \
    Model/blanknode.h \
    Model/Assembly/outeventportidl.h \
    Model/Assembly/ineventportidl.h \
    Model/Assembly/ineventportidl.h \
    Model/Assembly/outeventportidl.h \
    Model/Workload/ineventport.h \
    Model/Workload/outeventport.h \
    Model/Assembly/member.h \
    Controller/newcontroller.h \
    GUI/graphmlitem.h \
    medeawindow.h \
    GUI/projectwindow.h \
    GUI/filterbutton.h

SOURCES += \
    Controller/graphmlcontroller.cpp \
    GUI/attributetablemodel.cpp \
    GUI/nodeconnection.cpp \
    GUI/nodeitem.cpp \
    GUI/nodeview.cpp \
    Model/Assembly/componentassembly.cpp \
    Model/Assembly/componentinstance.cpp \
    Model/Assembly/eventport.cpp \
    Model/Assembly/hardwarecluster.cpp \
    Model/Assembly/hardwarenode.cpp \
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
    qlogger.cpp \
    Model/Assembly/attribute.cpp \
    Model/Workload/periodicevent.cpp \
    Model/Workload/component.cpp \
    Model/blanknode.cpp \
    Model/Assembly/outeventportidl.cpp \
    Model/Assembly/ineventportidl.cpp \
    Model/Assembly/ineventportidl.cpp \
    Model/Assembly/outeventportidl.cpp \
    Model/Workload/ineventport.cpp \
    Model/Workload/outeventport.cpp \
    Model/Assembly/member.cpp \
    Controller/newcontroller.cpp \
    GUI/graphmlitem.cpp \
    medeawindow.cpp \
    GUI/projectwindow.cpp \
    GUI/filterbutton.cpp

FORMS += \
    medeawindow.ui \
    mainwindow.ui
