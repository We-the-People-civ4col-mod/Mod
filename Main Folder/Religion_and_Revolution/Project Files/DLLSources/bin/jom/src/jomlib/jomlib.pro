TEMPLATE = lib
TARGET = jomlib
DESTDIR = ../../lib
QT = core
CONFIG += qt staticlib debug_and_release build_all
DEFINES += _CRT_SECURE_NO_WARNINGS
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
DEFINES += QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS
PRECOMPILED_HEADER = stable.h

build_pass:CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

isEmpty(MSYSPATH):MSYSPATH=C:\\msys
!exists($$MSYSPATH): MSYSPATH=D:\\msys
!exists($$MSYSPATH) {
    !build_pass:message("Can't locate path to MSYS. This is needed for flex.")
} else:!exists($$MSYSPATH\\1.0\\bin\\flex.exe) {
    !build_pass:message("MSYSPATH is set but flex cannot be found.")
} else {
    # One special extra compiler for ppexpr.l because
    # msys flex does not understand backslashes and I have no way
    # to translate the slashes in ${QMAKE_FILE_IN}. /me rolls eyes...
    PPEXPR_FLEX_FILE = $$PWD/ppexpr.l
    flex.name = flex ppexpr.l
    flex.input = PPEXPR_FLEX_FILE
    flex.output = ${QMAKE_FILE_BASE}-lex.inc
    flex.commands = $$MSYSPATH\\1.0\\bin\\flex.exe --noline $$PPEXPR_FLEX_FILE
    flex.CONFIG += no_link explicit_dependencies
    QMAKE_EXTRA_COMPILERS += flex

    QLALR_FILES = ppexpr.g
    qlalr.name = qlalr
    qlalr.input = QLALR_FILES
    qlalr.output = ${QMAKE_FILE_BASE}_grammar.cpp
    qlalr.commands = $$[QT_INSTALL_BINS]\\qlalr.exe --no-lines ${QMAKE_FILE_IN}
    qlalr.depends = $$PWD/${QMAKE_FILE_BASE}.l
    qlalr.dependency_type = TYPE_C
    qlalr.CONFIG += no_link explicit_dependencies
    QMAKE_EXTRA_COMPILERS += qlalr
}

win32-* {
    HEADERS +=  \
        iocompletionport.h
    SOURCES += \
        process.cpp \
        iocompletionport.cpp
} else {
    DEFINES += USE_QPROCESS
    SOURCES += \
        process_qt.cpp
}

HEADERS +=  \
    fastfileinfo.h \
    filetime.h \
    helperfunctions.h \
    makefile.h \
    makefilefactory.h \
    makefilelinereader.h \
    macrotable.h \
    exception.h \
    dependencygraph.h \
    options.h \
    parser.h \
    preprocessor.h \
    ppexprparser.h \
    targetexecutor.h \
    commandexecutor.h \
    process.h \
    processenvironment.h

SOURCES += \
    fastfileinfo.cpp \
    filetime.cpp \
    helperfunctions.cpp \
    macrotable.cpp \
    makefile.cpp \
    makefilefactory.cpp \
    makefilelinereader.cpp \
    exception.cpp \
    dependencygraph.cpp \
    options.cpp \
    parser.cpp \
    preprocessor.cpp \
    ppexpr_grammar.cpp \
    ppexprparser.cpp \
    targetexecutor.cpp \
    commandexecutor.cpp

OTHER_FILES += \
    ppexpr.g \
    ppexpr.l
