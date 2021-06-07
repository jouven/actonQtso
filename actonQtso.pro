#message($$QMAKESPEC)
QT -= gui

TEMPLATE = lib

!android:QMAKE_CXXFLAGS += -std=c++17
android:QMAKE_CXXFLAGS += -std=c++14
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

DEFINES += ACTONQTSO_LIB

HEADERS += \
    actionData.hpp \
    actionExecution/createDirectoryExecution.hpp \
    actionExecution/folderChangeReactionExecution.hpp \
    actionExecution/runProcessExecution.hpp \
    actionMappings/actionMapping.hpp \
    actions/createDirectory.hpp \
    actions/folderChangeReaction.hpp \
    actions/runProcess.hpp \
    actionMappings/actionExecutionStates.hpp \
    actionMappings/actionExecutionStateStrMapping.hpp \
    actionMappings/actions.hpp \
    actionMappings/actionStrMapping.hpp \
    checkExecution/actionStartedExecutingExecution.hpp \
    checkMappings/checkMapping.hpp \
    checkMappings/checks.hpp \
    actionDataExecutionResult.hpp \
    checkData.hpp \
    checkMappings/checkExecutionStates.hpp \
    checkMappings/checkExecutionStateStrMapping.hpp \
    checkMappings/checkStrMapping.hpp \
    checkDataExecutionResult.hpp \
    checks/actionStartedExecuting.hpp \
    checks/sameFile.hpp \
    checkExecution/actionFinishedExecution.hpp \
    checkExecution/sameFileExecution.hpp \
    actionExecution/baseActionExecution.hpp \
    checkExecution/baseCheckExecution.hpp \
    actonBaseSerialization.hpp \
    checks/actionFinished.hpp \
    actonExecutionOptions.hpp \
    actonDataHub.hpp \
    checks/timer.hpp \
    checksBaseSerialization.hpp \
    checksDataHub.hpp \
    crossPlatformMacros.hpp \
    actions/copyFile.hpp \
    actionExecution/copyFileExecution.hpp \
    dateTimeStringFormat.hpp \
    executionMessage.hpp \
    executionResult.hpp \
    reused/sameFileAlgo.hpp \
    actions/deleteFileDir.hpp \
    actionExecution/deleteFileDirExecution.hpp \
    checks/pathExists.hpp \
    checkExecution/pathExistsExecution.hpp \
    actionExecution/metaEndExecutionCycleExecution.hpp \
    actions/metaEndExecutionCycle.hpp \
    checkExecution/timerExecution.hpp \
    reused/stringAlgo.hpp

SOURCES += \
    actionData.cpp \
    actionExecution/createDirectoryExecution.cpp \
    actionExecution/folderChangeReactionExecution.cpp \
    actionExecution/runProcessExecution.cpp \
    actionMappings/actionMapping.cpp \
    actions/createDirectory.cpp \
    actions/folderChangeReaction.cpp \
    actions/runProcess.cpp \
    actionMappings/actionExecutionStateStrMapping.cpp \
    actionMappings/actionStrMapping.cpp \
    actionDataExecutionResult.cpp \
    checkData.cpp \
    checkExecution/actionStartedExecutingExecution.cpp \
    checkMappings/checkExecutionStateStrMapping.cpp \
    checkMappings/checkMapping.cpp \
    checkMappings/checkStrMapping.cpp \
    checkDataExecutionResult.cpp \
    checks/actionStartedExecuting.cpp \
    checks/sameFile.cpp \
    checkExecution/actionFinishedExecution.cpp \
    checkExecution/sameFileExecution.cpp \
    actonBaseSerialization.cpp \
    checks/actionFinished.cpp \
    actonExecutionOptions.cpp \
    actonDataHub.cpp \
    checks/timer.cpp \
    checksDataHub.cpp \
    checksBaseSerialization.cpp \
    checkExecution/baseCheckExecution.cpp \
    actionExecution/baseActionExecution.cpp \
    actions/copyFile.cpp \
    actionExecution/copyFileExecution.cpp \
    dateTimeStringFormat.cpp \
    executionMessage.cpp \
    executionResult.cpp \
    reused/sameFileAlgo.cpp \
    actions/deleteFileDir.cpp \
    actionExecution/deleteFileDirExecution.cpp \
    checks/pathExists.cpp \
    checkExecution/pathExistsExecution.cpp \
    actionExecution/metaEndExecutionCycleExecution.cpp \
    actions/metaEndExecutionCycle.cpp \
    checkExecution/timerExecution.cpp \
    reused/stringAlgo.cpp

!win32:MYPATH = "/"
win32:MYPATH = "H:/veryuseddata/portable/msys64/"

#mine
INCLUDEPATH += $${MYPATH}home/jouven/mylibs/include

if (!android){
#don't new line the "{"
#release
CONFIG(release, debug|release){
    LIBS += -L$${MYPATH}home/jouven/mylibs/release/
    DEPENDPATH += $${MYPATH}home/jouven/mylibs/release
    QMAKE_RPATHDIR += $${MYPATH}home/jouven/mylibs/release
}
#debug
CONFIG(debug, debug|release){
    LIBS += -L$${MYPATH}home/jouven/mylibs/debug/
    DEPENDPATH += $${MYPATH}home/jouven/mylibs/debug
    QMAKE_RPATHDIR += $${MYPATH}home/jouven/mylibs/debug
    DEFINES += DEBUGJOUVEN
}

}

if (android){
#release
CONFIG(release, debug|release){
    LIBS += -L$${MYPATH}home/jouven/mylibsAndroid/release/
    DEPENDPATH += $${MYPATH}home/jouven/mylibsAndroid/release
    QMAKE_RPATHDIR += $${MYPATH}home/jouven/mylibsAndroid/release
}
#debug
CONFIG(debug, debug|release){
    LIBS += -L$${MYPATH}home/jouven/mylibsAndroid/debug/
    DEPENDPATH += $${MYPATH}home/jouven/mylibsAndroid/debug
    QMAKE_RPATHDIR += $${MYPATH}home/jouven/mylibsAndroid/debug
    DEFINES += DEBUGJOUVEN

}

}

LIBS += -lbaseClassQtso -lthreadedFunctionQtso -llogsinJSONQtso \
    -lessentialQtso -lfilterDirectoryQtso -lstringParserMapQtso \
    -ltextQtso -lcryptoQtso

QMAKE_CXXFLAGS_DEBUG -= -g
QMAKE_CXXFLAGS_DEBUG += -pedantic -Wall -Wextra -g3

#CXXFLAGS
linux:QMAKE_CXXFLAGS_RELEASE += -flto=jobserver
win32::QMAKE_CXXFLAGS_RELEASE += -flto
!android:QMAKE_CXXFLAGS_RELEASE += -mtune=sandybridge

#for -flto=jobserver in the link step to work with -jX
linux:!android:QMAKE_LINK = +g++

linux:QMAKE_LFLAGS += -fuse-ld=gold
QMAKE_LFLAGS_RELEASE += -fvisibility=hidden
#LFLAGS
linux:QMAKE_LFLAGS_RELEASE += -flto=jobserver
win32::QMAKE_LFLAGS_RELEASE += -flto


#remove at some point, this is to suppress a qtcreator clang model issue
unix {
    INCLUDEPATH += /usr/lib/gcc/x86_64-linux-gnu/10/include
}
