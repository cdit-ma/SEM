include(../defaults.pri)

TEMPLATE = app
TARGET = MEDEA

#Output into a common directory
DESTDIR = ../MEDEA

#Link against the MEDEA_lib
LIBS += -L../src
LIBS += -lMEDEA_lib



REL_LIB_PATH = $$DESTDIR/../src/
win32{
    REL_LIB_PATH = $${REL_LIB_PATH}MEDEA_lib.lib
}linux-g++ | macx{
    REL_LIB_PATH = $${REL_LIB_PATH}libMEDEA_lib.a
}

#Setting this forces a relink as part of compilation
PRE_TARGETDEPS += $$REL_LIB_PATH


SOURCES += main.cpp


#Setup Icons
win32{
    #Used for Icon in windows.
    RC_ICONS += ../src/Resources/Images/MedeaIcon.ico
    #Sets Icon for GraphML Documents.
    RC_ICONS += ../src/Resources/Images/Graphml.ico
    LIBS += -lpsapi
}macx{
    macx:ICON = ../src/Resources/Images/MedeaIcon.icns
}

DISTFILES += \
    ../lib/Jenkins/jenkins-cli.jar \
    ../lib/Xalan/serializer.jar \
    ../lib/Xalan/xalan.jar \
    ../lib/Xerces/xercesImpl.jar \
    ../lib/Xerces/xml-apis.jar \
    ../lib/Jenkins/LICENSE.txt \
    ../lib/Xalan/LICENSE-2.0.txt \
    ../lib/Xerces/LICENSE-2.0.txt \
    ../transforms/IconPalette/PaletteOrg.graphml \
    ../transforms/IconPalette/WEDMLpalette.graphml \
    ../transforms/MEDEA.sch \
    ../transforms/OARIS PSM XMI.xml \
    ../transforms/graphml.dtd \
    ../transforms/mga.dtd \
    ../transforms/Absolute2Relative.xsl \
    ../transforms/Deploy.xsl \
    ../transforms/ExtractSchFromRNG.xsl \
    ../transforms/ExtractSchFromXSD.xsl \
    ../transforms/graphml2cdd.xsl \
    ../transforms/graphml2cdp.xsl \
    ../transforms/graphml2cpp.xsl \
    ../transforms/graphml2ddd.xsl \
    ../transforms/graphml2deployconfig.xsl \
    ../transforms/graphml2dpd.xsl \
    ../transforms/graphml2dpdold.xsl \
    ../transforms/graphml2dts.xsl \
    ../transforms/graphml2h.xsl \
    ../transforms/graphml2idl.xsl \
    ../transforms/graphml2mpc.xsl \
    ../transforms/graphml2mwc.xsl \
    ../transforms/graphml2testconfig.xsl \
    ../transforms/graphmlKeyAttributes.xsl \
    ../transforms/graphmlKeyVariables.xsl \
    ../transforms/iso_abstract_expand.xsl \
    ../transforms/iso_dsdl_include.xsl \
    ../transforms/iso_schematron_message.xsl \
    ../transforms/iso_schematron_skeleton_for_xslt1.xsl \
    ../transforms/iso_svrl_for_xslt1.xsl \
    ../transforms/MEDEA.xsl \
    ../transforms/picml2graphml.xsl \
    ../transforms/PreprocessIDL.xsl \
    ../transforms/Relative2Absolute.xsl \
    ../transforms/Replicate.xsl \
    ../transforms/Scale.xsl \
    ../transforms/xmi2graphml.xsl \
    ../transforms/xmi2xml.xsl \
    ../transforms/yEdAdd.xsl \
    ../transforms/yEdDelete.xsl \
    ../transforms/IconPalette/Aggregate.png \
    ../transforms/IconPalette/Assembly.png \
    ../transforms/IconPalette/Attribute.png \
    ../transforms/IconPalette/BranchState.png \
    ../transforms/IconPalette/Callback.png \
    ../transforms/IconPalette/Complexity.png \
    ../transforms/IconPalette/Component.png \
    ../transforms/IconPalette/Condition.png \
    ../transforms/IconPalette/cpu.png \
    ../transforms/IconPalette/db.png \
    ../transforms/IconPalette/Event.png \
    ../transforms/IconPalette/HardwareCluster.png \
    ../transforms/IconPalette/HardwareNode.png \
    ../transforms/IconPalette/hdd.png \
    ../transforms/IconPalette/IDL.png \
    ../transforms/IconPalette/InEventPort.png \
    ../transforms/IconPalette/InputParameter.png \
    ../transforms/IconPalette/log.png \
    ../transforms/IconPalette/Member.png \
    ../transforms/IconPalette/OutEventPort.png \
    ../transforms/IconPalette/PeriodicTask.png \
    ../transforms/IconPalette/Process.png \
    ../transforms/IconPalette/ProvidedRequestPort.png \
    ../transforms/IconPalette/ram.png \
    ../transforms/IconPalette/RequiredRequestPort.png \
    ../transforms/IconPalette/ReturnParameter.png \
    ../transforms/IconPalette/Termination.png \
    ../transforms/IconPalette/Variable.png \
    ../transforms/IconPalette/Vector.png \
    ../transforms/IconPalette/WhileLoop.png \
    ../transforms/IconPalette/Workload.png \
    ../transforms/schematron-skeleton-api.htm \
    ../transforms/README.txt \
    ../transforms/SchematronBuild.txt \
    ../transforms/SchematronReadme.txt \
    ../scripts/runCuts.pl \
    ../scripts/Jenkins_Construct_GraphMLNodesList.groovy


#Combine the Output Directory and the target directory.
OUTPUT_DIR = $$OUT_PWD/$$DESTDIR/
linux-g++ | win32{
    OUTPUT_DIR = $$OUTPUT_DIR/resources/
}
macx{
    OUTPUT_DIR = $$OUTPUT_DIR/Contents/MacOS/resources/
}

#Set the relative path to the files to copy
SCRIPTS_FILES.files = ../scripts/
LIB_FILES.files = ../lib/
TRANSFORMS_FILES.files = ../transforms/


#Set the output path
linux-g++ | win32{
    SCRIPTS_FILES.path =$$OUTPUT_DIR
    LIB_FILES.path = $$OUTPUT_DIR
    TRANSFORMS_FILES.path = $$OUTPUT_DIR
}
macx{
    SCRIPTS_FILES.path = $$OUTPUT_DIR/scripts/
    LIB_FILES.path = $$OUTPUT_DIR/lib/
    TRANSFORMS_FILES.path = $$OUTPUT_DIR/transforms/
}

#Copy files for Windows and Linux
linux-g++ | win32{
    INSTALLS += SCRIPTS_FILES
    INSTALLS += LIB_FILES
    INSTALLS += TRANSFORMS_FILES
}
macx{
    QMAKE_BUNDLE_DATA += SCRIPTS_FILES
    QMAKE_BUNDLE_DATA += LIB_FILES
    QMAKE_BUNDLE_DATA += TRANSFORMS_FILES
}

