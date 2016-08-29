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


VERSION = 1.5.2
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

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
    Controller/controller.h \
    Controller/doublehash.h \
    GUI/actionbutton.h \
    GUI/appsettings.h \
    GUI/codeeditor.h \
    GUI/keyeditwidget.h \
    GUI/shortcutdialog.h \
    GUI/syntaxhighlighter.h \
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
    Model/data.h \
    Model/edge.h \
    Model/entity.h \
    Model/graphml.h \
    Model/key.h \
    Model/model.h \
    Model/node.h \
    Model/tempentity.h \
    Model/workerdefinitions.h \
    View/Table/attributetablemodel.h \
    View/Validate/validatedialog.h \
    View/nodeviewminimap.h \
    View/theme.h \
    enumerations.h \
    modeltester.h \
    Plugins/CUTS/GUI/cutsexecutionwidget.h \
    Plugins/CUTS/cutsmanager.h \
    Plugins/Jenkins/GUI/jenkinsjobmonitorwidget.h \
    Plugins/Jenkins/GUI/jenkinsloadingwidget.h \
    Plugins/Jenkins/GUI/jenkinsstartjobwidget.h \
    Plugins/Jenkins/jenkinsmanager.h \
    Plugins/Jenkins/jenkinsrequest.h \
    Plugins/XMI/xmiimporter.h \
    Plugins/XMI/xmitreemodel.h \
    Plugins/XMI/GUI/XMITreeViewDialog.h \
    View/viewitem.h \
    Controller/viewcontroller.h \
    View/nodeviewitem.h \
    View/edgeviewitem.h \
    View/SceneItems/defaultnodeitem.h \
    View/SceneItems/edgeitemnew.h \
    View/SceneItems/entityitemnew.h \
    View/SceneItems/nodeitemnew.h \
    View/SceneItems/aspectitemnew.h \
    View/SceneItems/modelitemnew.h \
    View/nodeviewnew.h \
    Controller/selectionhandler.h \
    View/SceneItems/Hardware/hardwarenodeitem.h \
    View/docktitlebarwidget.h \
    Widgets/New/medeawindownew.h \
    Widgets/New/medeadockwidget.h \
    Widgets/New/medeawindowmanager.h \
    Widgets/New/medeamainwindow.h \
    Widgets/New/medeaviewdockwidget.h \
    Widgets/New/medeatooldockwidget.h \
    Controller/qobjectregistrar.h \
    Widgets/New/medeacentralwindow.h \
    Widgets/New/medeasubwindow.h \
    Widgets/New/medeaviewwindow.h \
    Widgets/New/tablewidget.h \
    Controller/actioncontroller.h \
    Widgets/New/actiongroup.h \
    Controller/rootaction.h \
    View/Toolbar/toolbarwidgetnew.h \
    View/SceneItems/Assemblies/managementcomponentnodeitem.h \
    Controller/toolbarcontroller.h \
    Controller/nodeviewitemaction.h \
    View/Table/multilinedelegate.h \
    Widgets/New/qosbrowser.h \
    Widgets/New/qosprofilemodel.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_historyqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_deadlineqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_destinationorderqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_durabilityqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_durabilityserviceqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_entityfactoryqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_groupdataqospolicy.h \
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
    Model/DeploymentDefinitions/QOS/DDS/dds_userdataqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_writerdatalifecycleqospolicy.h \
    Model/DeploymentDefinitions/QOS/DDS/dds_transportpriorityqospolicy.h \
    Model/Edges/qosedge.h \
    View/SceneItems/eventportnodeitem.h \
    View/Table/attributetableview.h \
    Widgets/New/medeanodeviewdockwidget.h \
    Controller/settingscontroller.h \
    View/SceneItems/attributenodeitem.h \
    Widgets/New/dataeditwidget.h \
    Controller/filehandler.h \
    Widgets/New/selectioncontroller.h \
    GUI/popupwidget.h \
    View/Dock/docktabwidget.h \
    View/Dock/dockactionwidget.h \
    View/SceneItems/nodeitemcontainer.h \
    View/SceneItems/Assemblies/nodeitemcolumncontainer.h \
    View/SceneItems/Assemblies/nodeitemcolumnitem.h \
    View/SceneItems/Assemblies/nodeitemstackcontainer.h \
    View/SceneItems/Assemblies/nodeitemorderedcontainer.h \
    View/SceneItems/containerelementnodeitem.h \
    View/SceneItems/Assemblies/assemblyeventportnodeitem.h \
    View/Dock/dockwidget.h

SOURCES += \
    main.cpp \
    Controller/controller.cpp \
    Controller/doublehash.cpp \
    GUI/actionbutton.cpp \
    GUI/appsettings.cpp \
    GUI/codeeditor.cpp \
    GUI/keyeditwidget.cpp \
    GUI/shortcutdialog.cpp \
    GUI/syntaxhighlighter.cpp \
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
    Model/data.cpp \
    Model/edge.cpp \
    Model/entity.cpp \
    Model/graphml.cpp \
    Model/key.cpp \
    Model/model.cpp \
    Model/node.cpp \
    Model/tempentity.cpp \
    Model/workerdefinitions.cpp \
    View/Table/attributetablemodel.cpp \
    View/Validate/validatedialog.cpp \
    View/nodeviewminimap.cpp \
    View/theme.cpp \
    enumerations.cpp \
    modeltester.cpp \
    Plugins/CUTS/GUI/cutsexecutionwidget.cpp \
    Plugins/CUTS/cutsmanager.cpp \
    Plugins/Jenkins/GUI/jenkinsjobmonitorwidget.cpp \
    Plugins/Jenkins/GUI/jenkinsloadingwidget.cpp \
    Plugins/Jenkins/GUI/jenkinsstartjobwidget.cpp \
    Plugins/Jenkins/jenkinsmanager.cpp \
    Plugins/Jenkins/jenkinsrequest.cpp \
    Plugins/XMI/xmiimporter.cpp \
    Plugins/XMI/xmitreemodel.cpp \
    Plugins/XMI/GUI/XMITreeViewDialog.cpp \
    View/viewitem.cpp \
    Controller/viewcontroller.cpp \
    View/nodeviewitem.cpp \
    View/edgeviewitem.cpp \
    View/SceneItems/defaultnodeitem.cpp \
    View/SceneItems/edgeitemnew.cpp \
    View/SceneItems/entityitemnew.cpp \
    View/SceneItems/nodeitemnew.cpp \
    View/SceneItems/aspectitemnew.cpp \
    View/SceneItems/modelitemnew.cpp \
    View/nodeviewnew.cpp \
    Controller/selectionhandler.cpp \
    View/SceneItems/Hardware/hardwarenodeitem.cpp \
    View/docktitlebarwidget.cpp \
    Widgets/New/medeawindownew.cpp \
    Widgets/New/medeadockwidget.cpp \
    Widgets/New/medeawindowmanager.cpp \
    Widgets/New/medeamainwindow.cpp \
    Widgets/New/medeaviewdockwidget.cpp \
    Widgets/New/medeatooldockwidget.cpp \
    Widgets/New/selectioncontroller.cpp \
    Controller/qobjectregistrar.cpp \
    Widgets/New/medeacentralwindow.cpp \
    Widgets/New/medeasubwindow.cpp \
    Widgets/New/medeaviewwindow.cpp \
    Widgets/New/tablewidget.cpp \
    Controller/actioncontroller.cpp \
    Widgets/New/actiongroup.cpp \
    Controller/rootaction.cpp \
    View/Toolbar/toolbarwidgetnew.cpp \
    View/SceneItems/Assemblies/managementcomponentnodeitem.cpp \
    Controller/toolbarcontroller.cpp \
    Controller/nodeviewitemaction.cpp \
    View/SceneItems/eventportnodeitem.cpp \
    View/Table/multilinedelegate.cpp \
    Widgets/New/qosbrowser.cpp \
    Widgets/New/qosprofilemodel.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_historyqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_deadlineqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_destinationorderqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_durabilityqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_durabilityserviceqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_entityfactoryqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_groupdataqospolicy.cpp \
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
    Model/DeploymentDefinitions/QOS/DDS/dds_userdataqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_writerdatalifecycleqospolicy.cpp \
    Model/DeploymentDefinitions/QOS/DDS/dds_transportpriorityqospolicy.cpp \
    Model/Edges/qosedge.cpp \
    View/Table/attributetableview.cpp \
    Widgets/New/medeanodeviewdockwidget.cpp \
    Controller/settingscontroller.cpp \
    View/SceneItems/attributenodeitem.cpp \
    Widgets/New/dataeditwidget.cpp \
    Controller/filehandler.cpp \
    GUI/popupwidget.cpp \
    View/Dock/docktabwidget.cpp \
    View/Dock/dockactionwidget.cpp \
    View/SceneItems/nodeitemcontainer.cpp \
    View/SceneItems/Assemblies/nodeitemcolumncontainer.cpp \
    View/SceneItems/Assemblies/nodeitemcolumnitem.cpp \
    View/SceneItems/Assemblies/nodeitemstackcontainer.cpp \
    View/SceneItems/Assemblies/nodeitemorderedcontainer.cpp \
    View/SceneItems/containerelementnodeitem.cpp \
    View/SceneItems/Assemblies/assemblyeventportnodeitem.cpp \
    View/Dock/dockwidget.cpp

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
