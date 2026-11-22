QT += core gui widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    terminal_backend.cpp

HEADERS += \
    mainwindow.h \
    terminal_backend.h

win32: LIBS += -ldwmapi -luser32

RC_ICONS = app.ico

