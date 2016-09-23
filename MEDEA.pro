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

VERSION = 2.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_URL=\\\"https://github.com/cdit-ma/MEDEA/\\\"

QMAKE_TARGET_COMPANY = CDIT-MA
QMAKE_TARGET_PRODUCT = MEDEA
QMAKE_TARGET_DESCRIPTION = MEDEA

TARGET = MEDEA
TEMPLATE = app

win32{
    #Used for Icon in windows.
    RC_ICONS += Resources/Images/MedeaIcon.ico
    #Sets Icon for GraphML Documents.
    RC_ICONS += Resources/Images/Graphml.ico
    LIBS += -lpsapi
}
mac{
    macx:ICON = $${PWD}/Resources/Images/MedeaIcon.icns
}

QMAKE_CFLAGS += -Wunused

#DEBUG MODE FLAG
#DEFINES += DEBUG_MODE

HEADERS += \
    Controllers/ActionController/actioncontroller.h \
    Controllers/SelectionController/selectioncontroller.h \
    Controllers/SelectionController/selectionhandler.h \
    Controllers/ToolbarController/nodeviewitemaction.h \
    Controllers/ToolbarController/toolbarcontroller.h \
    Controllers/ViewController/edgeviewitem.h \
    Controllers/ViewController/nodeviewitem.h \
    Controllers/ViewController/viewcontroller.h \
    Controllers/ViewController/viewitem.h \
    Controllers/modelcontroller.h \
    Controllers/SettingsController/settingscontroller.h \
    Controllers/SettingsController/setting.h \
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
    Model/BehaviourDefinitions/workerprocess.h \
    Model/BehaviourDefinitions/workload.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_deadlineqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_destinationorderqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_durabilityqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_durabilityserviceqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_entityfactoryqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_groupdataqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_historyqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_latencybudgetqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_lifespanqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_livelinessqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_ownershipqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_ownershipstrengthqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_partitionqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_presentationqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_qosprofile.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_readerdatalifecycleqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_reliabilityqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_resourcelimitsqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_timebasedfilterqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_topicdataqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_transportpriorityqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_userdataqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_writerdatalifecycleqospolicy.h \
    Model/DeploymentDefinitions/assemblydefinitions.h \
    Model/DeploymentDefinitions/attributeinstance.h \
    Model/DeploymentDefinitions/blackboxinstance.h \
    Model/DeploymentDefinitions/componentassembly.h \
    Model/DeploymentDefinitions/componentinstance.h \
    Model/DeploymentDefinitions/deploymentdefinitions.h \
    Model/DeploymentDefinitions/eventportdelegate.h \
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
    Model/Edges/qosedge.h \
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
    Model/data.h \
    Model/edge.h \
    Model/entity.h \
    Model/graphml.h \
    Model/key.h \
    Model/model.h \
    Model/node.h \
    Model/tempentity.h \
    Model/workerdefinitions.h \
    Plugins/CUTS/Widgets/cutsexecutionwidget.h \
    Plugins/CUTS/cutsmanager.h \
    Plugins/Jenkins/Widgets/jenkinsjobmonitorwidget.h \
    Plugins/Jenkins/Widgets/jenkinsloadingwidget.h \
    Plugins/Jenkins/Widgets/jenkinsstartjobwidget.h \
    Plugins/Jenkins/jenkinsmanager.h \
    Plugins/Jenkins/jenkinsrequest.h \
    Plugins/XMI/Widgets/XMITreeViewDialog.h \
    Plugins/XMI/xmiimporter.h \
    Plugins/XMI/xmitreemodel.h \
    Utils/actiongroup.h \
    Utils/filehandler.h \
    Utils/qobjectregistrar.h \
    Views/ContextToolbar/contexttoolbar.h \
    Views/Dock/docktabwidget.h \
    Views/Dock/dockwidget.h \
    Views/Dock/dockwidgetactionitem.h \
    Views/Dock/dockwidgetitem.h \
    Views/Dock/dockwidgetparentactionitem.h \
    Views/NodeView/SceneItems/Edge/edgeitem.h \
    Views/NodeView/SceneItems/Node/basicnodeitem.h \
    Views/NodeView/SceneItems/Node/defaultnodeitem.h \
    Views/NodeView/SceneItems/Node/hardwarenodeitem.h \
    Views/NodeView/SceneItems/Node/managementcomponentnodeitem.h \
    Views/NodeView/SceneItems/Node/nodeitem.h \
    Views/NodeView/SceneItems/Node/stacknodeitem.h \
    Views/NodeView/SceneItems/entityitem.h \
    Views/NodeView/nodeview.h \
    Views/NodeView/nodeviewminimap.h \
    Views/QOSBrowser/qosbrowser.h \
    Views/QOSBrowser/qosprofilemodel.h \
    Views/Search/searchdialog.h \
    Views/Search/searchitemwidget.h \
    Views/Table/datatablemodel.h \
    Views/Table/datatableview.h \
    Views/Table/datatablewidget.h \
    Widgets/CodeEditor/codebrowser.h \
    Widgets/CodeEditor/codeeditor.h \
    Widgets/CodeEditor/syntaxhighlighter.h \
    Widgets/Dialogs/appsettings.h \
    Widgets/Dialogs/modelvalidationdialog.h \
    Widgets/Dialogs/notificationdialog.h \
    Widgets/Dialogs/popupwidget.h \
    Widgets/Dialogs/shortcutdialog.h \
    Widgets/ViewManager/viewmanagerwidget.h \
    Widgets/Windows/welcomescreenwidget.h \
    Widgets/dataeditwidget.h \
    Widgets/keyeditwidget.h \
    enumerations.h \
    theme.h \
    Widgets/Windows/centralwindow.h \
    Widgets/Windows/mainwindow.h \
    Widgets/Windows/subwindow.h \
    Widgets/Windows/basewindow.h \
    Widgets/Windows/viewwindow.h \
    Widgets/DockWidgets/basedockwidget.h \
    Widgets/DockWidgets/docktitlebar.h \
    Widgets/DockWidgets/nodeviewdockwidget.h \
    Widgets/DockWidgets/tooldockwidget.h \
    Widgets/DockWidgets/viewdockwidget.h \
    Utils/rootaction.h \
    Utils/doublehash.h \
    Controllers/WindowManager/windowmanager.h \
    Views/Table/datatabledelegate.h \
    Widgets/ViewManager/windowitem.h \
    Widgets/ViewManager/dockitem.h


SOURCES += \
    Controllers/ActionController/actioncontroller.cpp \
    Controllers/SelectionController/selectioncontroller.cpp \
    Controllers/SelectionController/selectionhandler.cpp \
    Controllers/ToolbarController/nodeviewitemaction.cpp \
    Controllers/ToolbarController/toolbarcontroller.cpp \
    Controllers/ViewController/edgeviewitem.cpp \
    Controllers/ViewController/nodeviewitem.cpp \
    Controllers/ViewController/viewcontroller.cpp \
    Controllers/ViewController/viewitem.cpp \
    Controllers/WindowManager/windowmanager.cpp \
    Controllers/modelcontroller.cpp \
    Controllers/SettingsController/settingscontroller.cpp \
    Controllers/SettingsController/setting.cpp \
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
    Model/BehaviourDefinitions/workerprocess.cpp \
    Model/BehaviourDefinitions/workload.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_deadlineqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_destinationorderqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_durabilityqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_durabilityserviceqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_entityfactoryqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_groupdataqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_historyqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_latencybudgetqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_lifespanqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_livelinessqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_ownershipqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_ownershipstrengthqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_partitionqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_presentationqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_qosprofile.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_readerdatalifecycleqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_reliabilityqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_resourcelimitsqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_timebasedfilterqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_topicdataqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_transportpriorityqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_userdataqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_writerdatalifecycleqospolicy.cpp \
    Model/DeploymentDefinitions/assemblydefinitions.cpp \
    Model/DeploymentDefinitions/attributeinstance.cpp \
    Model/DeploymentDefinitions/blackboxinstance.cpp \
    Model/DeploymentDefinitions/componentassembly.cpp \
    Model/DeploymentDefinitions/componentinstance.cpp \
    Model/DeploymentDefinitions/deploymentdefinitions.cpp \
    Model/DeploymentDefinitions/eventportdelegate.cpp \
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
    Model/Edges/qosedge.cpp \
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
    Model/data.cpp \
    Model/edge.cpp \
    Model/entity.cpp \
    Model/graphml.cpp \
    Model/key.cpp \
    Model/model.cpp \
    Model/node.cpp \
    Model/tempentity.cpp \
    Model/workerdefinitions.cpp \
    Plugins/CUTS/Widgets/cutsexecutionwidget.cpp \
    Plugins/CUTS/cutsmanager.cpp \
    Plugins/Jenkins/Widgets/jenkinsjobmonitorwidget.cpp \
    Plugins/Jenkins/Widgets/jenkinsloadingwidget.cpp \
    Plugins/Jenkins/Widgets/jenkinsstartjobwidget.cpp \
    Plugins/Jenkins/jenkinsmanager.cpp \
    Plugins/Jenkins/jenkinsrequest.cpp \
    Plugins/XMI/Widgets/XMITreeViewDialog.cpp \
    Plugins/XMI/xmiimporter.cpp \
    Plugins/XMI/xmitreemodel.cpp \
    Utils/actiongroup.cpp \
    Utils/filehandler.cpp \
    Utils/qobjectregistrar.cpp \
    Views/ContextToolbar/contexttoolbar.cpp \
    Views/Dock/docktabwidget.cpp \
    Views/Dock/dockwidget.cpp \
    Views/Dock/dockwidgetactionitem.cpp \
    Views/Dock/dockwidgetitem.cpp \
    Views/Dock/dockwidgetparentactionitem.cpp \
    Views/NodeView/SceneItems/Edge/edgeitem.cpp \
    Views/NodeView/SceneItems/Node/basicnodeitem.cpp \
    Views/NodeView/SceneItems/Node/defaultnodeitem.cpp \
    Views/NodeView/SceneItems/Node/hardwarenodeitem.cpp \
    Views/NodeView/SceneItems/Node/managementcomponentnodeitem.cpp \
    Views/NodeView/SceneItems/Node/nodeitem.cpp \
    Views/NodeView/SceneItems/Node/stacknodeitem.cpp \
    Views/NodeView/SceneItems/entityitem.cpp \
    Views/NodeView/nodeview.cpp \
    Views/NodeView/nodeviewminimap.cpp \
    Views/QOSBrowser/qosbrowser.cpp \
    Views/QOSBrowser/qosprofilemodel.cpp \
    Views/Search/searchdialog.cpp \
    Views/Search/searchitemwidget.cpp \
    Views/Table/datatablemodel.cpp \
    Views/Table/datatableview.cpp \
    Views/Table/datatablewidget.cpp \
    Widgets/CodeEditor/codebrowser.cpp \
    Widgets/CodeEditor/codeeditor.cpp \
    Widgets/CodeEditor/syntaxhighlighter.cpp \
    Widgets/Dialogs/appsettings.cpp \
    Widgets/Dialogs/modelvalidationdialog.cpp \
    Widgets/Dialogs/notificationdialog.cpp \
    Widgets/Dialogs/popupwidget.cpp \
    Widgets/Dialogs/shortcutdialog.cpp \
    Widgets/ViewManager/viewmanagerwidget.cpp \
    Widgets/Windows/welcomescreenwidget.cpp \
    Widgets/dataeditwidget.cpp \
    Widgets/keyeditwidget.cpp \
    enumerations.cpp \
    main.cpp \
    theme.cpp \
    Widgets/Windows/centralwindow.cpp \
    Widgets/Windows/mainwindow.cpp \
    Widgets/Windows/subwindow.cpp \
    Widgets/Windows/viewwindow.cpp \
    Widgets/Windows/basewindow.cpp \
    Widgets/DockWidgets/docktitlebar.cpp \
    Widgets/DockWidgets/basedockwidget.cpp \
    Widgets/DockWidgets/nodeviewdockwidget.cpp \
    Widgets/DockWidgets/viewdockwidget.cpp \
    Widgets/DockWidgets/tooldockwidget.cpp \
    Utils/rootaction.cpp \
    Views/Table/datatabledelegate.cpp \
    Widgets/ViewManager/windowitem.cpp \
    Widgets/ViewManager/dockitem.cpp


RESOURCES += \
    resources.qrc

OTHER_FILES += \
    doxygen.config \
    changelog.txt \
    Resources/Scripts/jenkins-cli.jar \
    Resources/Scripts/Jenkins_Construct_GraphMLNodesList.groovy \
    Resources/Scripts/MEDEA.xsl \
    Resources/Scripts/serializer.jar \
    Resources/Scripts/xalan.jar \
    Resources/Scripts/xercesImpl.jar \
    Resources/Scripts/xml-apis.jar \
    Resources/Scripts/serializer.jar \
    Resources/Scripts/xalan.jar \
    Resources/Scripts/xercesImpl.jar \
    Resources/Scripts/xml-apis.jar \
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
    Resources/Transforms/graphml2dpdold.xsl \
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
    Resources/Scripts/runCuts.pl \
    installer/config/config.xml \
    installer/packages/cditma.MEDEA/meta/package.xml \
    installer/packages/cditma.MEDEA/meta/license.txt \
    installer/packages/cditma.MEDEA/meta/installscript.qs \
    Resources/Transforms/umi2xml.xsl



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

    QMAKE_BUNDLE_DATA += CHANGELOG_FILE
    QMAKE_BUNDLE_DATA += SCRIPTS_FILES
    QMAKE_BUNDLE_DATA += BINARIES_FILES
    QMAKE_BUNDLE_DATA += TRANSFORMS_FILES
}


FORMS += \
    installer/packages/cditma.MEDEA/meta/registerfilecheckboxform.ui
