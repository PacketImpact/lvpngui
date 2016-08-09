#-------------------------------------------------
#
# Project created by QtCreator 2016-07-24T18:15:54
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lvpngui
TEMPLATE = app


SOURCES += main.cpp\
    vpngui.cpp \
    installer.cpp \
    openvpn.cpp \
    pwstore.cpp \
    authdialog.cpp \
    logwindow.cpp \
    settingswindow.cpp

HEADERS  += \
    vpngui.h \
    installer.h \
    config.h \
    openvpn.h \
    pwstore.h \
    authdialog.h \
    logwindow.h \
    settingswindow.h

FORMS    += \
    authdialog.ui \
    logwindow.ui \
    settingswindow.ui

RESOURCES += \
    res.qrc

TRANSLATIONS += translations/lvpngui_fr.ts

QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++

DISTFILES += \
    lvpngui.manifest


# Crypto++
LIBPATH += C:/CryptoPP/release
INCLUDEPATH += C:/CryptoPP/include
LIBS += -lcryptopp


win32 {
    RC_FILE = lvpngui.rc
    LIBS += -lole32 -lshell32 -luuid
    WIN_PWD = $$replace(PWD, /, \\)
    OUT_PWD_WIN = $$replace(OUT_PWD, /, \\)
    QMAKE_POST_LINK = "$$quote(C:/Program Files/Microsoft SDKs/Windows/v6.0A/bin/mt.exe) -manifest $$quote($$WIN_PWD\\$$basename(TARGET).manifest) -outputresource:$$quote($$OUT_PWD_WIN\\${DESTDIR_TARGET};1)"
}
