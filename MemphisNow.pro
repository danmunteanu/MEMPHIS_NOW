QT += core gui widgets

TEMPLATE = app
TARGET = MemphisNow
CONFIG += c++17

SOURCES += \
    MemphisNowMain.cpp \
    MPSMainWindow.cpp \
    MPSTokenWidget.cpp

HEADERS += \
    MPSMainWindow.hpp \
    MPSTokenWidget.hpp

FORMS += MemphisNowWindow.ui
RESOURCES += MemphisNow.qrc

win32: RC_FILE = MemphisNow.rc

MEMPHIS_LIB_ROOT = $$clean_path($$PWD/../MEMPHIS_LIB)
BOOST_ROOT = $$clean_path($$PWD/../_LIBS/boost_1_92_0/mingw-install)

INCLUDEPATH += \
   $$MEMPHIS_LIB_ROOT \
   $$BOOST_ROOT/include/boost-1_92

LIBS += \
   -L$$MEMPHIS_LIB_ROOT/build \
   -lmemphis

MINGW_BIN = C:/Qt/Tools/mingw1310_64/bin
MEMPHIS_DLL = $$MEMPHIS_LIB_ROOT/build/memphis.dll
BOOST_FILESYSTEM_DLL = $$BOOST_ROOT/bin/libboost_filesystem-mgw13-mt-x64-1_92.dll
CONFIG(debug, debug|release) {
   APP_OUTPUT_DIR = $$OUT_PWD/debug
} else {
   APP_OUTPUT_DIR = $$OUT_PWD/release
}

QMAKE_POST_LINK += $$QMAKE_COPY_FILE \"$$system_path($$MEMPHIS_DLL)\" \"$$system_path($$APP_OUTPUT_DIR)\" $$escape_expand(\\n\\t)
QMAKE_POST_LINK += $$QMAKE_COPY_FILE \"$$system_path($$BOOST_FILESYSTEM_DLL)\" \"$$system_path($$APP_OUTPUT_DIR)\" $$escape_expand(\\n\\t)
QMAKE_POST_LINK += $$QMAKE_COPY_FILE \"$$system_path($$MINGW_BIN/libgcc_s_seh-1.dll)\" \"$$system_path($$APP_OUTPUT_DIR)\" $$escape_expand(\\n\\t)
QMAKE_POST_LINK += $$QMAKE_COPY_FILE \"$$system_path($$MINGW_BIN/libstdc++-6.dll)\" \"$$system_path($$APP_OUTPUT_DIR)\" $$escape_expand(\\n\\t)
QMAKE_POST_LINK += $$QMAKE_COPY_FILE \"$$system_path($$MINGW_BIN/libwinpthread-1.dll)\" \"$$system_path($$APP_OUTPUT_DIR)\"
