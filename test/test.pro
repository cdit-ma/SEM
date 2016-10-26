include(../defaults.pri)

QT += widgets testlib

CONFIG -= debug_and_release debug_and_release_target

HEADERS += \
    modelcontrollertests.h

SOURCES += \
    main.cpp \
    modelcontrollertests.cpp
	
#Link against the MEDEA_lib
LIBS += -L../src
LIBS += -lMEDEA_lib

RESOURCES += \
    resources.qrc
