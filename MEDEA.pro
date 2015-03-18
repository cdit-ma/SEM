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
medeawindow.h \
    GUI/attributetablemodel.h \
    GUI/nodeitem.h \
    GUI/nodeview.h \
    Model/edge.h \
    Model/graphml.h \
    Model/graphmldata.h \
    Model/graphmlkey.h \
    Model/model.h \
    Model/node.h \
    Model/blanknode.h \
    Controller/newcontroller.h \
    GUI/graphmlitem.h \
    Model/BehaviourDefinitions/periodicevent.h \
    Model/DeploymentDefinitions/componentassembly.h \
    Model/DeploymentDefinitions/componentinstance.h \
    Model/DeploymentDefinitions/hardwarecluster.h \
    Model/DeploymentDefinitions/hardwarenode.h \
    Model/BehaviourDefinitions/behaviourdefinitions.h \
    Model/DeploymentDefinitions/deploymentdefinitions.h \
    Model/InterfaceDefinitions/interfacedefinitions.h \
    Model/BehaviourDefinitions/componentimpl.h \
    Model/BehaviourDefinitions/ineventportimpl.h \
    Model/BehaviourDefinitions/outeventportimpl.h \
    Model/BehaviourDefinitions/attributeimpl.h \
    Model/InterfaceDefinitions/outeventport.h \
    Model/DeploymentDefinitions/outeventportinstance.h \
    Model/InterfaceDefinitions/component.h \
    Model/InterfaceDefinitions/ineventport.h \
    Model/DeploymentDefinitions/ineventportinstance.h \
    Model/InterfaceDefinitions/attribute.h \
    Model/DeploymentDefinitions/attributeinstance.h \
    Model/InterfaceDefinitions/file.h \
    Model/InterfaceDefinitions/aggregate.h \
    Model/InterfaceDefinitions/member.h \
    Model/InterfaceDefinitions/eventport.h \
    Model/BehaviourDefinitions/variable.h \
    Model/BehaviourDefinitions/process.h \
    Model/BehaviourDefinitions/workload.h \
    Model/BehaviourDefinitions/branchstate.h \
    Model/BehaviourDefinitions/condition.h \
    Model/BehaviourDefinitions/termination.h \
    Model/DeploymentDefinitions/assemblydefinitions.h \
    Model/DeploymentDefinitions/hardwaredefinitions.h \
    ValidationEngine/validationengine.h \
    ValidationEngine/validationplugin.h \
    ValidationEngine/Plugins/interfacedefinitionplugin.h \
    ValidationEngine/validationengine.h \
    ValidationEngine/Plugins/interfacedefinitionplugin.h \
    GUI/nodeedge.h \
    Model/InterfaceDefinitions/memberinstance.h \
    Model/InterfaceDefinitions/aggregateinstance.h \
    Model/InterfaceDefinitions/memberinstance.h \
    Model/DeploymentDefinitions/managementcomponent.h \
    Model/DeploymentDefinitions/ineventportdelegate.h \
    Model/DeploymentDefinitions/outeventportdelegate.h \
    modeltester.h \
    GUI/nodeviewminimap.h \
    GUI/toolbarwidget.h \
    GUI/toolbarwidgetaction.h \
    GUI/toolbarwidgetmenu.h \
    GUI/docknodeitem.h \
    GUI/partsdockscrollarea.h \
    GUI/hardwaredockscrollarea.h \
    GUI/definitionsdockscrollarea.h \
    GUI/graphmlviewitem.h \
    GUI/nodetable.h \
    GUI/Toolbar/toolbarwidget.h \
    GUI/Toolbar/toolbarwidgetaction.h \
    GUI/Toolbar/toolbarwidgetmenu.h \
    GUI/Dock/definitionsdockscrollarea.h \
    GUI/Dock/docknodeitem.h \
    GUI/Dock/dockscrollarea.h \
    GUI/Dock/docktogglebutton.h \
    GUI/Dock/hardwaredockscrollarea.h \
    GUI/Dock/partsdockscrollarea.h \
    GUI/editabletextitem.h \

SOURCES += \
    GUI/attributetablemodel.cpp \
    GUI/nodeitem.cpp \
    GUI/nodeview.cpp \
    Model/edge.cpp \
    Model/graphml.cpp \
    Model/graphmldata.cpp \
    Model/graphmlkey.cpp \
    Model/model.cpp \
    Model/node.cpp \
    main.cpp \
    Model/blanknode.cpp \
    Controller/newcontroller.cpp \
    GUI/graphmlitem.cpp \
    Model/BehaviourDefinitions/periodicevent.cpp \
    Model/DeploymentDefinitions/componentassembly.cpp \
    Model/DeploymentDefinitions/componentinstance.cpp \
    Model/DeploymentDefinitions/hardwarecluster.cpp \
    Model/DeploymentDefinitions/hardwarenode.cpp \
    Model/BehaviourDefinitions/behaviourdefinitions.cpp \
    Model/DeploymentDefinitions/deploymentdefinitions.cpp \
    Model/InterfaceDefinitions/interfacedefinitions.cpp \
    Model/BehaviourDefinitions/componentimpl.cpp \
    Model/BehaviourDefinitions/ineventportimpl.cpp \
    Model/BehaviourDefinitions/attributeimpl.cpp \
    Model/BehaviourDefinitions/outeventportimpl.cpp \
    Model/InterfaceDefinitions/outeventport.cpp \
    Model/DeploymentDefinitions/outeventportinstance.cpp \
    Model/InterfaceDefinitions/component.cpp \
    Model/DeploymentDefinitions/ineventportinstance.cpp \
    Model/InterfaceDefinitions/ineventport.cpp \
    Model/InterfaceDefinitions/attribute.cpp \
    Model/DeploymentDefinitions/attributeinstance.cpp \
    Model/InterfaceDefinitions/file.cpp \
    Model/InterfaceDefinitions/aggregate.cpp \
    Model/InterfaceDefinitions/member.cpp \
    Model/InterfaceDefinitions/eventport.cpp \
    Model/BehaviourDefinitions/variable.cpp \
    Model/BehaviourDefinitions/process.cpp \
    Model/BehaviourDefinitions/workload.cpp \
    Model/BehaviourDefinitions/branchstate.cpp \
    Model/BehaviourDefinitions/condition.cpp \
    Model/BehaviourDefinitions/termination.cpp \
    Model/DeploymentDefinitions/assemblydefinitions.cpp \
    Model/DeploymentDefinitions/hardwaredefinitions.cpp \
    ValidationEngine/validationengine.cpp \
    ValidationEngine/Plugins/interfacedefinitionplugin.cpp \
    ValidationEngine/validationplugin.cpp \
    ValidationEngine/Plugins/interfacedefinitionplugin.cpp \
    medeawindow.cpp \
    GUI/nodeedge.cpp \
    Model/InterfaceDefinitions/memberinstance.cpp \
    Model/InterfaceDefinitions/aggregateinstance.cpp \
    Model/InterfaceDefinitions/memberinstance.cpp \
    Model/DeploymentDefinitions/managementcomponent.cpp \
    Model/DeploymentDefinitions/ineventportdelegate.cpp \
    Model/DeploymentDefinitions/outeventportdelegate.cpp \
    modeltester.cpp \
    GUI/nodeviewminimap.cpp \
    GUI/graphmlviewitem.cpp \
    GUI/Toolbar/toolbarwidget.cpp \
    GUI/Toolbar/toolbarwidgetaction.cpp \
    GUI/Toolbar/toolbarwidgetmenu.cpp \
    GUI/Dock/definitionsdockscrollarea.cpp \
    GUI/Dock/docknodeitem.cpp \
    GUI/Dock/dockscrollarea.cpp \
    GUI/Dock/docktogglebutton.cpp \
    GUI/Dock/hardwaredockscrollarea.cpp \
    GUI/Dock/partsdockscrollarea.cpp \
    GUI/editabletextitem.cpp \

FORMS += \

RESOURCES += \
    resources.qrc

OTHER_FILES +=
