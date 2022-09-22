QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bullet.cpp \
    button.cpp \
    chatserver.cpp \
    enemy.cpp \
    graphicsview.cpp \
    gun.cpp \
    healthbar.cpp \
    main.cpp \
    mainwindow.cpp \
    player.cpp \
    serverworker.cpp

HEADERS += \
    bullet.h \
    button.h \
    chatserver.h \
    enemy.h \
    graphicsview.h \
    gun.h \
    healthbar.h \
    mainwindow.h \
    player.h \
    serverworker.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
