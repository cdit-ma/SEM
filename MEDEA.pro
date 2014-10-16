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

CONFIG += CONSOLE

TEMPLATE = app

HEADERS += \
    GUI/attributetablemodel.h \
    GUI/nodeconnection.h \
    GUI/nodeitem.h \
    GUI/nodeview.h \
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
    Model/blanknode.h \
    Controller/newcontroller.h \
    GUI/graphmlitem.h \
    medeawindow.h \
    GUI/projectwindow.h \
    GUI/filterbutton.h \
    Model/BehaviourDefinitions/component.h \
    Model/BehaviourDefinitions/ineventport.h \
    Model/BehaviourDefinitions/outeventport.h \
    Model/BehaviourDefinitions/periodicevent.h \
    Model/DeploymentDefinitions/attribute.h \
    Model/DeploymentDefinitions/componentassembly.h \
    Model/DeploymentDefinitions/componentinstance.h \
    Model/DeploymentDefinitions/eventport.h \
    Model/DeploymentDefinitions/hardwarecluster.h \
    Model/DeploymentDefinitions/hardwarenode.h \
    Model/DeploymentDefinitions/ineventportidl.h \
    Model/DeploymentDefinitions/member.h \
    Model/DeploymentDefinitions/outeventportidl.h \
    Model/BehaviourDefinitions/behaviourdefinitions.h \
    Model/DeploymentDefinitions/deploymentdefinitions.h \
    Model/InterfaceDefinitions/interfacedefinitions.h

SOURCES += \
    GUI/attributetablemodel.cpp \
    GUI/nodeconnection.cpp \
    GUI/nodeitem.cpp \
    GUI/nodeview.cpp \
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
    Model/blanknode.cpp \
    Controller/newcontroller.cpp \
    GUI/graphmlitem.cpp \
    medeawindow.cpp \
    GUI/projectwindow.cpp \
    GUI/filterbutton.cpp \
    Model/BehaviourDefinitions/component.cpp \
    Model/BehaviourDefinitions/ineventport.cpp \
    Model/BehaviourDefinitions/outeventport.cpp \
    Model/BehaviourDefinitions/periodicevent.cpp \
    Model/DeploymentDefinitions/attribute.cpp \
    Model/DeploymentDefinitions/componentassembly.cpp \
    Model/DeploymentDefinitions/componentinstance.cpp \
    Model/DeploymentDefinitions/eventport.cpp \
    Model/DeploymentDefinitions/hardwarecluster.cpp \
    Model/DeploymentDefinitions/hardwarenode.cpp \
    Model/DeploymentDefinitions/ineventportidl.cpp \
    Model/DeploymentDefinitions/member.cpp \
    Model/DeploymentDefinitions/outeventportidl.cpp \
    Model/BehaviourDefinitions/behaviourdefinitions.cpp \
    Model/DeploymentDefinitions/deploymentdefinitions.cpp \
    Model/InterfaceDefinitions/interfacedefinitions.cpp

FORMS += \
    medeawindow.ui \
    mainwindow.ui
