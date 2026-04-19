HEADERS += \
    $$PWD/mainwindow.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp

FORMS += \
    $$PWD/mainwindow.ui

#UI页面
INCLUDEPATH += $$PWD/ui
include     ($$PWD/ui/ui.pri)
