#message($$QMAKESPEC)
QT = widgets

TARGET = actonQtso
TEMPLATE = lib

!android:QMAKE_CXXFLAGS += -std=c++17
android:CONFIG += c++14
CONFIG += no_keywords plugin
#(only windows) fixes the extra tier of debug and release build directories inside the first build directories
win32:CONFIG -= debug_and_release

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

!win32:MYPATH = "/"
win32:MYPATH = "H:/veryuseddata/portable/msys64/"

#mine
INCLUDEPATH += $${MYPATH}home/jouven/mylibs/include

QMAKE_CXXFLAGS_DEBUG -= -g
QMAKE_CXXFLAGS_DEBUG += -pedantic -Wall -Wextra -g3

#if not win32, add flto, mingw (on msys2) can't handle lto
linux:QMAKE_CXXFLAGS_RELEASE += -flto=jobserver
!android:QMAKE_CXXFLAGS_RELEASE += -mtune=sandybridge

#for -flto=jobserver in the link step to work with -j4
linux:!android:QMAKE_LINK = +g++

linux:QMAKE_LFLAGS += -fuse-ld=gold
QMAKE_LFLAGS_RELEASE += -fvisibility=hidden
#if not win32, add flto, mingw (on msys2) can't handle lto
linux:QMAKE_LFLAGS_RELEASE += -flto=jobserver

HEADERS += \
    actionData.hpp \
    actionExecution/createDirectoryExecution.hpp \
    actionExecution/runProcessExecution.hpp \
    actions/createDirectory.hpp \
    actions/runProcess.hpp \
    actionMappings/actionExecutionStates.hpp \
    actionMappings/actionExecutionStateStrMapping.hpp \
    actionMappings/actions.hpp \
    actionMappings/actionStrMapping.hpp \
    checkMappings/checks.hpp \
    actionDataExecutionResult.hpp \
    checkData.hpp \
    checkMappings/checkExecutionStates.hpp \
    checkMappings/checkExecutionStateStrMapping.hpp \
    checkMappings/checkStrMapping.hpp \
    checkDataExecutionResult.hpp \
    checks/sameFile.hpp \
    checkExecution/actionFinishedExecution.hpp \
    checkExecution/sameFileExecution.hpp \
    actionExecution/baseActionExecution.hpp \
    checkExecution/baseCheckExecution.hpp \
    actonBaseSerialization.hpp \
    checks/actionFinished.hpp \
    actonExecutionOptions.hpp \
    actonDataHub.hpp \
    checksBaseSerialization.hpp \
    checksDataHub.hpp

SOURCES += \
    actionData.cpp \
    actionExecution/createDirectoryExecution.cpp \
    actionExecution/runProcessExecution.cpp \
    actions/createDirectory.cpp \
    actions/runProcess.cpp \
    actionMappings/actionExecutionStateStrMapping.cpp \
    actionMappings/actionStrMapping.cpp \
    actionDataExecutionResult.cpp \
    checkData.cpp \
    checkMappings/checkExecutionStateStrMapping.cpp \
    checkMappings/checkStrMapping.cpp \
    checkDataExecutionResult.cpp \
    checks/sameFile.cpp \
    checkExecution/actionFinishedExecution.cpp \
    checkExecution/sameFileExecution.cpp \
    actonBaseSerialization.cpp \
    checks/actionFinished.cpp \
    actonExecutionOptions.cpp \
    actonDataHub.cpp \
    checksDataHub.cpp \
    checksBaseSerialization.cpp \
    checkExecution/baseCheckExecution.cpp \
    actionExecution/baseActionExecution.cpp

if (!android){
#don't new line the "{"
#release
CONFIG(release, debug|release){

}
#debug
CONFIG(debug, debug|release){

    DEFINES += DEBUGJOUVEN
}
}

if (android){
#release
CONFIG(release, debug|release){

}
#debug
CONFIG(debug, debug|release){

    DEFINES += DEBUGJOUVEN
}

}
