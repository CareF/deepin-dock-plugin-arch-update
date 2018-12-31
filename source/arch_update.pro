QT              += widgets svg gui
TEMPLATE         = lib
CONFIG          += plugin c++11
 
TARGET           = $$qtLibraryTarget(arch_update)
DESTDIR          = $$_PRO_FILE_PWD_
DISTFILES       += arch_update.json

HEADERS += \
    archupdateplugin.h \ 
    archupdatedata.h \
    archupdateitem.h \
    archupdateapplet.h \
    settingdiag.h
 
SOURCES += \
    archupdateplugin.cpp \ 
    archupdatedata.cpp \
    archupdateitem.cpp \
    archupdateapplet.cpp \
    settingdiag.cpp
 
isEmpty(PREFIX) {
    PREFIX = /usr
}
 
target.path = $${PREFIX}/lib/dde-dock/plugins/
INSTALLS += target

RESOURCES += \
    archupdate.qrc
