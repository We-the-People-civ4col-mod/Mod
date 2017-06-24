TEMPLATE = app
QT += testlib
CONFIG += depend_includepath
INCLUDEPATH += ../src/jomlib
DEFINES += SRCDIR=\\\"$$PWD/\\\"

CONFIG(debug, debug|release) {
    TARGET = testsd
} else {
    TARGET = tests
}

PROJECT_BUILD_ROOT=$$OUT_PWD/..
include(../src/jomlib/use_jomlib.pri)

contains(QMAKE_CXXFLAGS_RELEASE, -MT) {
    QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:msvcrt
}

HEADERS += tests.h
SOURCES += tests.cpp

