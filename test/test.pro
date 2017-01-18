include(../defaults.pri)

QT += widgets testlib

CONFIG -= debug_and_release debug_and_release_target

HEADERS += \
    modelcontrollertests.h \
    viewcontrollertests.h \
    modelelementtests.h \
    AdoptionTests/modeladoptiontest.h \
    AdoptionTests/interfacedefinitionsadoptiontest.h

SOURCES += \
    main.cpp \
    modelcontrollertests.cpp \
    viewcontrollertests.cpp \
    modelelementtests.cpp \
    AdoptionTests/modeladoptiontest.cpp \
    AdoptionTests/interfacedefinitionsadoptiontest.cpp

#Link against the MEDEA_lib
LIBS += -L../src
LIBS += -lMEDEA_lib

REL_LIB_PATH = ../src/
win32{
    REL_LIB_PATH = $${REL_LIB_PATH}MEDEA_lib.lib
}linux-g++ | macx{
    REL_LIB_PATH = $${REL_LIB_PATH}libMEDEA_lib.a
}

#Setting this forces a relink as part of compilation
PRE_TARGETDEPS += $$REL_LIB_PATH

RESOURCES += \
    resources.qrc

DISTFILES += \
    Resources/Models/HelloWorld (copy).graphml \
    Resources/Models/HelloWorld (copy).graphml
