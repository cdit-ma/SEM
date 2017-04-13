include(../defaults.pri)

TEMPLATE = app
TARGET = MEDEA

#Output into a common directory
DESTDIR = ../MEDEA/

#Link against the MEDEA_lib
LIB_DIR = ../src
LIB_NAME = MEDEA_lib

#Link Against MEDEA_lib
LIBS += -L$${LIB_DIR}
LIBS += -l$${LIB_NAME}

win32{
    #On Windows *.lib
    LIB_PATH = $${LIB_DIR}/$${LIB_NAME}.lib
}
linux-g++ | macx{
    #On Unix lib*.a
    LIB_PATH = $${LIB_DIR}/lib$${LIB_NAME}.a
}

#Setting this forces a relink as part of compilation
PRE_TARGETDEPS += $${LIB_PATH}

ROOT_DIR = $$PWD/../
SRC_RESOURCE_DIR = $${ROOT_DIR}/src/Resources

#Setup Icons
win32{
    #Used for Icon in windows.
    RC_ICONS += $${SRC_RESOURCE_DIR}/medea.ico
    #Sets Icon for GraphML Documents.
    RC_ICONS += $${SRC_RESOURCE_DIR}/graphml.ico
    LIBS += -lpsapi
}macx{
    macx:ICON = $${SRC_RESOURCE_DIR}/medea.icns
}

#Set the sources
SOURCES += main.cpp

linux-g++ | win32{
    RESOURCE_DIR = $${OUT_PWD}/$${DESTDIR}/Resources
}
macx{
    RESOURCE_DIR = $${OUT_PWD}/$${DESTDIR}/$${TARGET}.app/Contents/MacOS/Resources
}



message("LIB: $${ROOT_DIR}/lib")
message("RESOURCE: $${RESOURCE_DIR}")
#Copy Libs, Transforms and Scripts
mk_resource.commands += $(MKDIR) $${RESOURCE_DIR}
copy_libs.commands += $(COPY_DIR) $${ROOT_DIR}/lib $${RESOURCE_DIR}/lib
copy_transforms.commands += $(COPY_DIR) $${ROOT_DIR}/transforms $${RESOURCE_DIR}/transforms
copy_scripts.commands += $(COPY_DIR) $${ROOT_DIR}/scripts $${RESOURCE_DIR}/scripts
first.depends = $(first) mk_resource copy_libs copy_transforms copy_scripts
QMAKE_EXTRA_TARGETS += first mk_resource copy_libs copy_transforms copy_scripts


