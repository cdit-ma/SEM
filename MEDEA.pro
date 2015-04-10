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

TEMPLATE = app

#DEFINES += DEBUG_MODE

LIBS += -lpsapi

HEADERS += \
    Controller/controller.h \
    Model/BehaviourDefinitions/attributeimpl.h \
    Model/BehaviourDefinitions/behaviourdefinitions.h \
    Model/BehaviourDefinitions/branchstate.h \
    Model/BehaviourDefinitions/componentimpl.h \
    Model/BehaviourDefinitions/condition.h \
    Model/BehaviourDefinitions/ineventportimpl.h \
    Model/BehaviourDefinitions/outeventportimpl.h \
    Model/BehaviourDefinitions/periodicevent.h \
    Model/BehaviourDefinitions/process.h \
    Model/BehaviourDefinitions/termination.h \
    Model/BehaviourDefinitions/variable.h \
    Model/BehaviourDefinitions/workload.h \
    Model/DeploymentDefinitions/assemblydefinitions.h \
    Model/DeploymentDefinitions/attributeinstance.h \
    Model/DeploymentDefinitions/componentassembly.h \
    Model/DeploymentDefinitions/componentinstance.h \
    Model/DeploymentDefinitions/deploymentdefinitions.h \
    Model/DeploymentDefinitions/hardwarecluster.h \
    Model/DeploymentDefinitions/hardwaredefinitions.h \
    Model/DeploymentDefinitions/hardwarenode.h \
    Model/DeploymentDefinitions/ineventportdelegate.h \
    Model/DeploymentDefinitions/ineventportinstance.h \
    Model/DeploymentDefinitions/managementcomponent.h \
    Model/DeploymentDefinitions/outeventportdelegate.h \
    Model/DeploymentDefinitions/outeventportinstance.h \
    Model/InterfaceDefinitions/aggregate.h \
    Model/InterfaceDefinitions/aggregateinstance.h \
    Model/InterfaceDefinitions/attribute.h \
    Model/InterfaceDefinitions/component.h \
    Model/InterfaceDefinitions/eventport.h \
    Model/InterfaceDefinitions/file.h \
    Model/InterfaceDefinitions/ineventport.h \
    Model/InterfaceDefinitions/interfacedefinitions.h \
    Model/InterfaceDefinitions/member.h \
    Model/InterfaceDefinitions/memberinstance.h \
    Model/InterfaceDefinitions/outeventport.h \
    Model/blanknode.h \
    Model/edge.h \
    Model/graphml.h \
    Model/graphmlcontainer.h \
    Model/graphmldata.h \
    Model/graphmlkey.h \
    Model/model.h \
    Model/node.h \
    View/Dock/definitionsdockscrollarea.h \
    View/Dock/docknodeitem.h \
    View/Dock/dockscrollarea.h \
    View/Dock/docktogglebutton.h \
    View/Dock/hardwaredockscrollarea.h \
    View/Dock/partsdockscrollarea.h \
    View/GraphicsItems/edgeitem.h \
    View/GraphicsItems/editabletextitem.h \
    View/GraphicsItems/graphmlitem.h \
    View/GraphicsItems/nodeitem.h \
    View/Table/attributetablemodel.h \
    View/Table/comboboxtabledelegate.h \
    View/Toolbar/toolbarwidget.h \
    View/Toolbar/toolbarwidgetaction.h \
    View/Toolbar/toolbarwidgetmenu.h \
    View/nodeview.h \
    View/nodeviewminimap.h \
    medeawindow.h \
    modeltester.h \
    GUI/appsettings.h \
    GUI/keyeditwidget.h

SOURCES += \
    Controller/controller.cpp \
    Model/BehaviourDefinitions/attributeimpl.cpp \
    Model/BehaviourDefinitions/behaviourdefinitions.cpp \
    Model/BehaviourDefinitions/branchstate.cpp \
    Model/BehaviourDefinitions/componentimpl.cpp \
    Model/BehaviourDefinitions/condition.cpp \
    Model/BehaviourDefinitions/ineventportimpl.cpp \
    Model/BehaviourDefinitions/outeventportimpl.cpp \
    Model/BehaviourDefinitions/periodicevent.cpp \
    Model/BehaviourDefinitions/process.cpp \
    Model/BehaviourDefinitions/termination.cpp \
    Model/BehaviourDefinitions/variable.cpp \
    Model/BehaviourDefinitions/workload.cpp \
    Model/DeploymentDefinitions/assemblydefinitions.cpp \
    Model/DeploymentDefinitions/attributeinstance.cpp \
    Model/DeploymentDefinitions/componentassembly.cpp \
    Model/DeploymentDefinitions/componentinstance.cpp \
    Model/DeploymentDefinitions/deploymentdefinitions.cpp \
    Model/DeploymentDefinitions/hardwarecluster.cpp \
    Model/DeploymentDefinitions/hardwaredefinitions.cpp \
    Model/DeploymentDefinitions/hardwarenode.cpp \
    Model/DeploymentDefinitions/ineventportdelegate.cpp \
    Model/DeploymentDefinitions/ineventportinstance.cpp \
    Model/DeploymentDefinitions/managementcomponent.cpp \
    Model/DeploymentDefinitions/outeventportdelegate.cpp \
    Model/DeploymentDefinitions/outeventportinstance.cpp \
    Model/InterfaceDefinitions/aggregate.cpp \
    Model/InterfaceDefinitions/aggregateinstance.cpp \
    Model/InterfaceDefinitions/attribute.cpp \
    Model/InterfaceDefinitions/component.cpp \
    Model/InterfaceDefinitions/eventport.cpp \
    Model/InterfaceDefinitions/file.cpp \
    Model/InterfaceDefinitions/ineventport.cpp \
    Model/InterfaceDefinitions/interfacedefinitions.cpp \
    Model/InterfaceDefinitions/member.cpp \
    Model/InterfaceDefinitions/memberinstance.cpp \
    Model/InterfaceDefinitions/outeventport.cpp \
    Model/blanknode.cpp \
    Model/edge.cpp \
    Model/graphml.cpp \
    Model/graphmldata.cpp \
    Model/graphmlkey.cpp \
    Model/model.cpp \
    Model/node.cpp \
    View/Dock/definitionsdockscrollarea.cpp \
    View/Dock/docknodeitem.cpp \
    View/Dock/dockscrollarea.cpp \
    View/Dock/docktogglebutton.cpp \
    View/Dock/hardwaredockscrollarea.cpp \
    View/Dock/partsdockscrollarea.cpp \
    View/GraphicsItems/edgeitem.cpp \
    View/GraphicsItems/editabletextitem.cpp \
    View/GraphicsItems/graphmlitem.cpp \
    View/GraphicsItems/nodeitem.cpp \
    View/Table/attributetablemodel.cpp \
    View/Table/comboboxtabledelegate.cpp \
    View/Toolbar/toolbarwidget.cpp \
    View/Toolbar/toolbarwidgetaction.cpp \
    View/Toolbar/toolbarwidgetmenu.cpp \
    View/nodeview.cpp \
    View/nodeviewminimap.cpp \
    main.cpp \
    medeawindow.cpp \
    modeltester.cpp \
    GUI/appsettings.cpp \
    GUI/keyeditwidget.cpp

FORMS += \

RESOURCES += \
    resources.qrc

OTHER_FILES +=
