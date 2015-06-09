#-------------------------------------------------
#
# Project created by QtCreator 2014-06-19T14:12:59
#
#-------------------------------------------------

QT       += core
QT       += xml
QT       += gui
QT       += widgets
QT       += xmlpatterns


TARGET = MEDEA

TEMPLATE = app

win32{
    LIBS += -lpsapi
}

#DEFINES += DEBUG_MODE

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
    Model/InterfaceDefinitions/ineventport.h \
    Model/InterfaceDefinitions/interfacedefinitions.h \
    Model/InterfaceDefinitions/member.h \
    Model/InterfaceDefinitions/memberinstance.h \
    Model/InterfaceDefinitions/outeventport.h \
    Model/blanknode.h \
    Model/edge.h \
    Model/graphml.h \
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
    GUI/keyeditwidget.h \
    GUI/searchitembutton.h \
    View/Validate/validatedialog.h \
    View/Toolbar/toolbarwidgetbutton.h \
	Model/InterfaceDefinitions/idl.h \
    GUI/aspecttogglewidget.h \
    Jenkins/GUI/jenkinsjobmonitorwidget.h \
    Jenkins/GUI/jenkinsloadingwidget.h \
    Jenkins/GUI/jenkinsstartjobwidget.h \
    Jenkins/jenkinsmanager.h \
    Jenkins/jenkinsrequest.h

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
    GUI/keyeditwidget.cpp \
    GUI/searchitembutton.cpp \
    View/Validate/validatedialog.cpp \
    View/Toolbar/toolbarwidgetbutton.cpp \
    Model/InterfaceDefinitions/idl.cpp \
    GUI/aspecttogglewidget.cpp \
    Jenkins/GUI/jenkinsjobmonitorwidget.cpp \
    Jenkins/GUI/jenkinsloadingwidget.cpp \
    Jenkins/GUI/jenkinsstartjobwidget.cpp \
    Jenkins/jenkinsmanager.cpp \
    Jenkins/jenkinsrequest.cpp

FORMS += \

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    Resources/Scripts/jenkins-cli.jar \
    Resources/Scripts/Jenkins_Construct_GraphMLNodesList.groovy \
    settings.ini \
    Resources/Scripts/MEDEA.xsl \
    Resources/Scripts/serializer.jar \
    Resources/Scripts/xalan.jar \
    Resources/Scripts/xercesImpl.jar \
    Resources/Scripts/xml-apis.jar \
    Resources/Scripts/serializer.jar \
    Resources/Scripts/xalan.jar \
    Resources/Scripts/xercesImpl.jar \
    Resources/Scripts/xml-apis.jar


#Define $$OUTPUT_DIR
win32{
    #Set the Output directory.
    CONFIG(release, debug|release) {
        OUTPUT_DIR = $$OUT_PWD/release/
    }

    CONFIG(debug, debug|release) {
        OUTPUT_DIR = $$OUT_PWD/debug/
    }
}

linux-g++{
    OUTPUT_DIR = $$OUT_PWD/
}

mac{
    OUTPUT_DIR = Contents/MacOS/
}


JENKINSCLIJAR.files += Resources/Scripts/jenkins-cli.jar
JENKINSCLIJAR.path = $$OUTPUT_DIR/Resources/Scripts/

JENKINSGROOVY.files += Resources/Scripts/Jenkins_Construct_GraphMLNodesList.groovy
JENKINSGROOVY.path = $$OUTPUT_DIR/Resources/Scripts/

SETTINGSFILE.files += settings.ini
SETTINGSFILE.path = $$OUTPUT_DIR/

XSLFILE.files += Resources/Scripts/MEDEA.xsl
XSLFILE.path = $$OUTPUT_DIR/Resources/Scripts/

XALANJAR.files += Resources/Scripts/xalan.jar
XALANJAR.path = $$OUTPUT_DIR/Resources/Scripts/

SERIALIZERJAR.files += Resources/Scripts/serializer.jar
SERIALIZERJAR.path = $$OUTPUT_DIR/Resources/Scripts/

XERCESIMPLJAR.files += Resources/Scripts/xercesImpl.jar
XERCESIMPLJAR.path = $$OUTPUT_DIR/Resources/Scripts/

XMLAPISJAR.files += Resources/Scripts/xml-apis.jar
XMLAPISJAR.path = $$OUTPUT_DIR/Resources/Scripts/

#Copy files for Windows and Linux
linux-g++ | win32{
    INSTALLS += JENKINSCLIJAR
    INSTALLS += JENKINSGROOVY
    INSTALLS += SETTINGSFILE
    INSTALLS += XSLFILE
    INSTALLS += XALANJAR
    INSTALLS += SERIALIZERJAR
    INSTALLS += XERCESIMPLJAR
    INSTALLS += XMLAPISJAR
}

#Copy files for MacOS
mac{
    QMAKE_BUNDLE_DATA += JENKINSCLIJAR
    QMAKE_BUNDLE_DATA += JENKINSGROOVY
    QMAKE_BUNDLE_DATA += SETTINGSFILE
    QMAKE_BUNDLE_DATA += XSLFILE
    QMAKE_BUNDLE_DATA += XALANJAR
    QMAKE_BUNDLE_DATA += SERIALIZERJAR
    QMAKE_BUNDLE_DATA += XERCESIMPLJAR
    QMAKE_BUNDLE_DATA += XMLAPISJAR
}
