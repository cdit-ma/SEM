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


VERSION = 1.5.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QMAKE_TARGET_COMPANY = Defence Information Group
QMAKE_TARGET_PRODUCT = MEDEA
QMAKE_TARGET_DESCRIPTION = Modelling, Experiment DEsign and Analysis


TARGET = MEDEA

TEMPLATE = app

win32{
    #Used for Icon in windows.
    RC_ICONS = Resources/Images/MedeaIcon.ico
    LIBS += -lpsapi
}
mac{
    macx:ICON = $${PWD}/Resources/Images/MedeaIcon.icns
}

QMAKE_CFLAGS += -Wunused

#DEBUG MODE FLAG
#DEFINES += DEBUG_MODE

HEADERS += \
    Controller/behaviournodeadapter.h \
    Controller/controller.h \
    Controller/doublehash.h \
    Controller/edgeadapter.h \
    Controller/entityadapter.h \
    Controller/nodeadapter.h \
    CUTS/GUI/cutsexecutionwidget.h \
    CUTS/cutsmanager.h \
    GUI/actionbutton.h \
    GUI/appsettings.h \
    GUI/aspecttogglewidget.h \
    GUI/codeeditor.h \
    GUI/keyeditwidget.h \
    GUI/searchdialog.h \
    GUI/searchitem.h \
    GUI/searchsuggestcompletion.h \
    GUI/shortcutdialog.h \
    GUI/syntaxhighlighter.h \
    Jenkins/GUI/jenkinsjobmonitorwidget.h \
    Jenkins/GUI/jenkinsloadingwidget.h \
    Jenkins/GUI/jenkinsstartjobwidget.h \
    Jenkins/jenkinsmanager.h \
    Jenkins/jenkinsrequest.h \
    Model/BehaviourDefinitions/attributeimpl.h \
    Model/BehaviourDefinitions/behaviourdefinitions.h \
    Model/BehaviourDefinitions/behaviournode.h \
    Model/BehaviourDefinitions/branch.h \
    Model/BehaviourDefinitions/branchstate.h \
    Model/BehaviourDefinitions/componentimpl.h \
    Model/BehaviourDefinitions/condition.h \
    Model/BehaviourDefinitions/eventportimpl.h \
    Model/BehaviourDefinitions/ineventportimpl.h \
    Model/BehaviourDefinitions/inputparameter.h \
    Model/BehaviourDefinitions/outeventportimpl.h \
    Model/BehaviourDefinitions/parameter.h \
    Model/BehaviourDefinitions/periodicevent.h \
    Model/BehaviourDefinitions/process.h \
    Model/BehaviourDefinitions/returnparameter.h \
    Model/BehaviourDefinitions/termination.h \
    Model/BehaviourDefinitions/variable.h \
    Model/BehaviourDefinitions/whileloop.h \
    Model/BehaviourDefinitions/workload.h \
    Model/DeploymentDefinitions/assemblydefinitions.h \
    Model/DeploymentDefinitions/attributeinstance.h \
    Model/DeploymentDefinitions/blackboxinstance.h \
    Model/DeploymentDefinitions/componentassembly.h \
    Model/DeploymentDefinitions/componentinstance.h \
    Model/DeploymentDefinitions/deploymentdefinitions.h \
    Model/DeploymentDefinitions/eventportdelegate.h \
    Model/DeploymentDefinitions/eventportinstance.h \
    Model/DeploymentDefinitions/hardware.h \
    Model/DeploymentDefinitions/hardwarecluster.h \
    Model/DeploymentDefinitions/hardwaredefinitions.h \
    Model/DeploymentDefinitions/hardwarenode.h \
    Model/DeploymentDefinitions/ineventportdelegate.h \
    Model/DeploymentDefinitions/ineventportinstance.h \
    Model/DeploymentDefinitions/managementcomponent.h \
    Model/DeploymentDefinitions/outeventportdelegate.h \
    Model/DeploymentDefinitions/outeventportinstance.h \
    Model/Edges/aggregateedge.h \
    Model/Edges/assemblyedge.h \
    Model/Edges/dataedge.h \
    Model/Edges/definitionedge.h \
    Model/Edges/deploymentedge.h \
    Model/Edges/workflowedge.h \
    Model/InterfaceDefinitions/aggregate.h \
    Model/InterfaceDefinitions/aggregateinstance.h \
    Model/InterfaceDefinitions/attribute.h \
    Model/InterfaceDefinitions/blackbox.h \
    Model/InterfaceDefinitions/component.h \
    Model/InterfaceDefinitions/datanode.h \
    Model/InterfaceDefinitions/eventport.h \
    Model/InterfaceDefinitions/idl.h \
    Model/InterfaceDefinitions/ineventport.h \
    Model/InterfaceDefinitions/interfacedefinitions.h \
    Model/InterfaceDefinitions/member.h \
    Model/InterfaceDefinitions/memberinstance.h \
    Model/InterfaceDefinitions/outeventport.h \
    Model/InterfaceDefinitions/vector.h \
    Model/InterfaceDefinitions/vectorinstance.h \
    Model/blanknode.h \
    Model/data.h \
    Model/edge.h \
    Model/entity.h \
    Model/graphml.h \
    Model/key.h \
    Model/model.h \
    Model/node.h \
    Model/tempentity.h \
    Model/workerdefinitions.h \
    View/Dock/definitionsdockscrollarea.h \
    View/Dock/docknodeitem.h \
    View/Dock/dockscrollarea.h \
    View/Dock/docktogglebutton.h \
    View/Dock/functionsdockscrollarea.h \
    View/Dock/hardwaredockscrollarea.h \
    View/Dock/partsdockscrollarea.h \
    View/GraphicsItems/aspectitem.h \
    View/GraphicsItems/edgeitem.h \
    View/GraphicsItems/edgeitemarrow.h \
    View/GraphicsItems/editabletextitem.h \
    View/GraphicsItems/entityitem.h \
    View/GraphicsItems/graphmlitem.h \
    View/GraphicsItems/inputitem.h \
    View/GraphicsItems/modelitem.h \
    View/GraphicsItems/nodeitem.h \
    View/GraphicsItems/noguiitem.h \
    View/GraphicsItems/notificationitem.h \
    View/GraphicsItems/statusitem.h \
    View/Table/attributetablemodel.h \
    View/Table/comboboxtabledelegate.h \
    View/Toolbar/toolbarmenu.h \
    View/Toolbar/toolbarmenuaction.h \
    View/Toolbar/toolbarwidget.h \
    View/Validate/validatedialog.h \
    View/nodeview.h \
    View/nodeviewminimap.h \
    View/theme.h \
    Widgets/medeasubwindow.h \
    Widgets/medeawindow.h \
    enumerations.h \
    modeltester.h


SOURCES += \
    main.cpp \
    Controller/behaviournodeadapter.cpp \
    Controller/controller.cpp \
    Controller/doublehash.cpp \
    Controller/edgeadapter.cpp \
    Controller/entityadapter.cpp \
    Controller/nodeadapter.cpp \
    CUTS/GUI/cutsexecutionwidget.cpp \
    CUTS/cutsmanager.cpp \
    GUI/actionbutton.cpp \
    GUI/appsettings.cpp \
    GUI/aspecttogglewidget.cpp \
    GUI/codeeditor.cpp \
    GUI/keyeditwidget.cpp \
    GUI/searchdialog.cpp \
    GUI/searchitem.cpp \
    GUI/searchsuggestcompletion.cpp \
    GUI/shortcutdialog.cpp \
    GUI/syntaxhighlighter.cpp \
    Jenkins/GUI/jenkinsjobmonitorwidget.cpp \
    Jenkins/GUI/jenkinsloadingwidget.cpp \
    Jenkins/GUI/jenkinsstartjobwidget.cpp \
    Jenkins/jenkinsmanager.cpp \
    Jenkins/jenkinsrequest.cpp \
    Model/BehaviourDefinitions/attributeimpl.cpp \
    Model/BehaviourDefinitions/behaviourdefinitions.cpp \
    Model/BehaviourDefinitions/behaviournode.cpp \
    Model/BehaviourDefinitions/branch.cpp \
    Model/BehaviourDefinitions/branchstate.cpp \
    Model/BehaviourDefinitions/componentimpl.cpp \
    Model/BehaviourDefinitions/condition.cpp \
    Model/BehaviourDefinitions/eventportimpl.cpp \
    Model/BehaviourDefinitions/ineventportimpl.cpp \
    Model/BehaviourDefinitions/inputparameter.cpp \
    Model/BehaviourDefinitions/outeventportimpl.cpp \
    Model/BehaviourDefinitions/parameter.cpp \
    Model/BehaviourDefinitions/periodicevent.cpp \
    Model/BehaviourDefinitions/process.cpp \
    Model/BehaviourDefinitions/returnparameter.cpp \
    Model/BehaviourDefinitions/termination.cpp \
    Model/BehaviourDefinitions/variable.cpp \
    Model/BehaviourDefinitions/whileloop.cpp \
    Model/BehaviourDefinitions/workload.cpp \
    Model/DeploymentDefinitions/assemblydefinitions.cpp \
    Model/DeploymentDefinitions/attributeinstance.cpp \
    Model/DeploymentDefinitions/blackboxinstance.cpp \
    Model/DeploymentDefinitions/componentassembly.cpp \
    Model/DeploymentDefinitions/componentinstance.cpp \
    Model/DeploymentDefinitions/deploymentdefinitions.cpp \
    Model/DeploymentDefinitions/eventportdelegate.cpp \
    Model/DeploymentDefinitions/eventportinstance.cpp \
    Model/DeploymentDefinitions/hardware.cpp \
    Model/DeploymentDefinitions/hardwarecluster.cpp \
    Model/DeploymentDefinitions/hardwaredefinitions.cpp \
    Model/DeploymentDefinitions/hardwarenode.cpp \
    Model/DeploymentDefinitions/ineventportdelegate.cpp \
    Model/DeploymentDefinitions/ineventportinstance.cpp \
    Model/DeploymentDefinitions/managementcomponent.cpp \
    Model/DeploymentDefinitions/outeventportdelegate.cpp \
    Model/DeploymentDefinitions/outeventportinstance.cpp \
    Model/Edges/aggregateedge.cpp \
    Model/Edges/assemblyedge.cpp \
    Model/Edges/dataedge.cpp \
    Model/Edges/definitionedge.cpp \
    Model/Edges/deploymentedge.cpp \
    Model/Edges/workflowedge.cpp \
    Model/InterfaceDefinitions/aggregate.cpp \
    Model/InterfaceDefinitions/aggregateinstance.cpp \
    Model/InterfaceDefinitions/attribute.cpp \
    Model/InterfaceDefinitions/blackbox.cpp \
    Model/InterfaceDefinitions/component.cpp \
    Model/InterfaceDefinitions/datanode.cpp \
    Model/InterfaceDefinitions/eventport.cpp \
    Model/InterfaceDefinitions/idl.cpp \
    Model/InterfaceDefinitions/ineventport.cpp \
    Model/InterfaceDefinitions/interfacedefinitions.cpp \
    Model/InterfaceDefinitions/member.cpp \
    Model/InterfaceDefinitions/memberinstance.cpp \
    Model/InterfaceDefinitions/outeventport.cpp \
    Model/InterfaceDefinitions/vector.cpp \
    Model/InterfaceDefinitions/vectorinstance.cpp \
    Model/blanknode.cpp \
    Model/data.cpp \
    Model/edge.cpp \
    Model/entity.cpp \
    Model/graphml.cpp \
    Model/key.cpp \
    Model/model.cpp \
    Model/node.cpp \
    Model/tempentity.cpp \
    Model/workerdefinitions.cpp \
    View/Dock/definitionsdockscrollarea.cpp \
    View/Dock/docknodeitem.cpp \
    View/Dock/dockscrollarea.cpp \
    View/Dock/docktogglebutton.cpp \
    View/Dock/functionsdockscrollarea.cpp \
    View/Dock/hardwaredockscrollarea.cpp \
    View/Dock/partsdockscrollarea.cpp \
    View/GraphicsItems/aspectitem.cpp \
    View/GraphicsItems/edgeitem.cpp \
    View/GraphicsItems/edgeitemarrow.cpp \
    View/GraphicsItems/editabletextitem.cpp \
    View/GraphicsItems/entityitem.cpp \
    View/GraphicsItems/graphmlitem.cpp \
    View/GraphicsItems/inputitem.cpp \
    View/GraphicsItems/modelitem.cpp \
    View/GraphicsItems/nodeitem.cpp \
    View/GraphicsItems/noguiitem.cpp \
    View/GraphicsItems/notificationitem.cpp \
    View/GraphicsItems/statusitem.cpp \
    View/Table/attributetablemodel.cpp \
    View/Table/comboboxtabledelegate.cpp \
    View/Toolbar/toolbarmenu.cpp \
    View/Toolbar/toolbarmenuaction.cpp \
    View/Toolbar/toolbarwidget.cpp \
    View/Validate/validatedialog.cpp \
    View/nodeview.cpp \
    View/nodeviewminimap.cpp \
    View/theme.cpp \
    Widgets/medeasubwindow.cpp \
    Widgets/medeawindow.cpp \
    enumerations.cpp \
    modeltester.cpp


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
    Resources/Scripts/xml-apis.jar \
    changelog.txt \
    Resources/Transforms/Absolute2Relative.xsl \
    Resources/Transforms/Deploy.xsl \
    Resources/Transforms/ExtractSchFromRNG.xsl \
    Resources/Transforms/ExtractSchFromXSD.xsl \
    Resources/Transforms/graphml2cdd.xsl \
    Resources/Transforms/graphml2cdp.xsl \
    Resources/Transforms/graphml2cpp.xsl \
    Resources/Transforms/graphml2ddd.xsl \
    Resources/Transforms/graphml2deployconfig.xsl \
    Resources/Transforms/graphml2dpd.xsl \
    Resources/Transforms/graphml2dts.xsl \
    Resources/Transforms/graphml2h.xsl \
    Resources/Transforms/graphml2idl.xsl \
    Resources/Transforms/graphml2mpc.xsl \
    Resources/Transforms/graphml2mwc.xsl \
    Resources/Transforms/graphml2testconfig.xsl \
    Resources/Transforms/graphmlKeyAttributes.xsl \
    Resources/Transforms/graphmlKeyVariables.xsl \
    Resources/Transforms/iso_abstract_expand.xsl \
    Resources/Transforms/iso_dsdl_include.xsl \
    Resources/Transforms/iso_schematron_message.xsl \
    Resources/Transforms/iso_schematron_skeleton_for_xslt1.xsl \
    Resources/Transforms/iso_svrl_for_xslt1.xsl \
    Resources/Transforms/MEDEA.xsl \
    Resources/Transforms/picml2graphml.xsl \
    Resources/Transforms/PreprocessIDL.xsl \
    Resources/Transforms/Relative2Absolute.xsl \
    Resources/Transforms/Scale.xsl \
    Resources/Transforms/yEdAdd.xsl \
    Resources/Transforms/yEdDelete.xsl \
    Resources/Transforms/graphml.dtd \
    Resources/Transforms/mga.dtd \
    Resources/Transforms/schematron-skeleton-api.htm \
    Resources/Transforms/MEDEA.sch \
    Resources/Transforms/README.txt \
    Resources/Transforms/SchematronBuild.txt \
    Resources/Transforms/SchematronReadme.txt \
    Resources/Binaries/jenkins-cli.jar \
    Resources/Binaries/serializer.jar \
    Resources/Binaries/xalan.jar \
    Resources/Binaries/xercesImpl.jar \
    Resources/Binaries/xml-apis.jar \
    Resources/Scripts/runCuts.pl


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




equals($$DEPLOY, "true") {
    SETTINGS_FILE.files += settings.ini
    SETTINGS_FILE.path = $$OUTPUT_DIR/
}else{
    SETTINGS_FILE.files += settings.ini
    SETTINGS_FILE.path = $$OUTPUT_DIR/
}


CHANGELOG_FILE.files += changelog.txt
CHANGELOG_FILE.path = $$OUTPUT_DIR/

SCRIPTS_FILES.files += Resources/Scripts/
BINARIES_FILES.files += Resources/Binaries/
TRANSFORMS_FILES.files += Resources/Transforms/

SCRIPTS_FILES.path = $$OUTPUT_DIR/Resources/
BINARIES_FILES.path = $$OUTPUT_DIR/Resources/
TRANSFORMS_FILES.path = $$OUTPUT_DIR/Resources/


#Copy files for Windows and Linux
linux-g++ | win32{
    INSTALLS += SETTINGS_FILE
    INSTALLS += CHANGELOG_FILE
    INSTALLS += SCRIPTS_FILES
    INSTALLS += BINARIES_FILES
    INSTALLS += TRANSFORMS_FILES
}

#Copy files for MacOS
mac{
    #MAC requires different directories.
    SCRIPTS_FILES.path = $$OUTPUT_DIR/Resources/Scripts/
    BINARIES_FILES.path = $$OUTPUT_DIR/Resources/Binaries/
    TRANSFORMS_FILES.path = $$OUTPUT_DIR/Resources/Transforms/
    WORKER_FILES.path = $$OUTPUT_DIR/Resources/WorkerDefinitions/

    QMAKE_BUNDLE_DATA += SETTINGS_FILE
    QMAKE_BUNDLE_DATA += CHANGELOG_FILE
    QMAKE_BUNDLE_DATA += SCRIPTS_FILES
    QMAKE_BUNDLE_DATA += BINARIES_FILES
    QMAKE_BUNDLE_DATA += TRANSFORMS_FILES
}

DISTFILES += \
    Resources/Images/qt.ico \
    Resources/Fonts/OpenSans-Regular.ttf \
    settings.ini \
    doxygen.config \
    defaultSettings.ini \
    Resources/Images/Actions/Export_Snippet.png \
    Resources/Images/Actions/Import_Snippet.png \
    README.md
