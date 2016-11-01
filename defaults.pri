VERSION = 2.0.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_URL=\\\"https://github.com/cdit-ma/MEDEA/\\\"

DEFINES += DEBUG_MODE

QMAKE_TARGET_COMPANY = CDIT-MA
QMAKE_TARGET_PRODUCT = MEDEA
QMAKE_TARGET_DESCRIPTION = Modelling, Experiment DEsign and Analysis (MEDEA)

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src
SRC_DIR = $$PWD

QT	+= core
QT	+= xml
QT	+= gui
QT	+= widgets
QT	+= xmlpatterns

CONFIG -= debug_and_release debug_and_release_target
