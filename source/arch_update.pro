QT              += widgets svg gui
TEMPLATE         = lib
CONFIG          += plugin c++11 lrelease embed_translations link_pkgconfig
PKGCONFIG       += dtkwidget
 
TARGET           = $$qtLibraryTarget(arch_update)
DESTDIR          = $$_PRO_FILE_PWD_
DISTFILES       += arch_update.json

HEADERS += \
    archupdateplugin.h \ 
    archupdatedata.h \
    archupdateitem.h \
    archupdateapplet.h \
    settingdialog.h
 
SOURCES += \
    archupdateplugin.cpp \ 
    archupdatedata.cpp \
    archupdateitem.cpp \
    archupdateapplet.cpp \
    settingdialog.cpp
 
isEmpty(PREFIX) {
    PREFIX = /usr
}
 
target.path = $${PREFIX}/lib/dde-dock/plugins/
INSTALLS += target

RESOURCES += \
    archupdate.qrc

TRANSLATIONS = $$_PRO_FILE_PWD_/i18n/archupdate-zh_CN.ts

#updatets.target = $$TRANSLATIONS
#updatets.commands = lupdate $$_PRO_FILE_
#updatets.depends += $$SOURCES $$HEADERS
#updateqm.target = i18n/archupdate.zh_CN.qm
#updateqm.commands = lrelease $$_PRO_FILE_
#updateqm.depends += $$TRANSLATIONS
#QMAKE_EXTRA_TARGETS += updatets updateqm

