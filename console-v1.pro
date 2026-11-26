QT += core gui widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    terminal_backend.cpp

HEADERS += \
    mainwindow.h \
    terminal_backend.h

win32 {
    DEFINES += _WIN32_WINNT=0x0A00 NTDDI_VERSION=0x0A000006
    LIBS += -ldwmapi -luser32 -lkernel32
}

unix: LIBS += -lutil

RC_ICONS = app.ico
