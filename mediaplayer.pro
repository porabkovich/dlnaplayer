#-------------------------------------------------
#
# Project created by QtCreator 2011-11-01T11:20:45
#
#-------------------------------------------------

QT      += core gui xml network

TARGET   = mediaplayer

TEMPLATE = lib

CONFIG  += plugin
CONFIG += QT BRISA
CONFIG += qt qxt
QXT += core web

BRISA += upnp core utils

linux-mips_sigma-g++	{
	QMAKE_CFLAGS_RELEASE -= -g
	QMAKE_CXXFLAGS_RELEASE -= -g

	QMAKE_CFLAGS  += -EL
	QMAKE_CXXFLAGS += -EL
	QMAKE_LFLAGS += -EL -Wl,-rpath=$$QMAKE_LIBDIR_QT
}
QMAKE_LFLAGS +=  -Wl,--unresolved-symbols=ignore-in-shared-libs

DEFINES += "GIT_VERSION=\"`git log --pretty=format:'%h %d' --abbrev-commit --date=short -1`\""
DEFINES += "PLUGIN_VERSION=1.1.43"
# if we on master branch, pass -DGLOB_NODEBUG, otherwise -DGLOB_DEBUG
DEFINES += "`if [ \\`git symbolic-ref HEAD|sed s^refs/heads/^^\\` != "master" ]; then echo DEBUG; else echo NDEBUG; fi`"

system(test "$STBAPI_PLATFORM" = "brcm"):DEFINES+=BRCM

INCLUDEPATH    += 	../common/interfaces \
			../common/classes \
			../common/other \
			../ext/inc

SOURCES += \
        ../common/other/tools.cpp \
        ../common/interfaces/copiertypes.cpp \
        ../common/classes/cfiletypedetect.cpp \
        ../common/classes/cscreenkeyboard.cpp \
        ../common/other/widgetgrabber.cpp \
        mediaplayer.cpp \
        cimagecacher.cpp \
        cmediainfoparser.cpp \
        dialogs.cpp \
        cinfobar.cpp \
        ciconprovider.cpp \
        mntres.cpp \
        cfileopendialog.cpp \
        csyscommandthread.cpp \
        copypanel.cpp \
        treeview.cpp \
        cfilecopierclient.cpp \
        csizemanager.cpp \
        cinfomessage.cpp \
        cprogressbar.cpp \
        caboutcontent.cpp \
        csharemounter.cpp \
        cnativesyscall.cpp \
        csharemanager.cpp \
        cshareeditdialog.cpp \
        cshareslistdialog.cpp \
    csharesparser.cpp \
    cperiodicalprocesscall.cpp \
    cautoshareswatcher.cpp \
    sharemigration.cpp \
    dlna.cpp


HEADERS  += \
        ../common/interfaces/stb_plugin_interface.h \
        ../common/interfaces/stb_application_plugin.h \
        ../common/other/debugmacros.h \
        ../common/classes/cfiletypedetect.h \
        ../common/classes/cscreenkeyboard.h \
        ../common/other/tools.h \
        ../common/interfaces/copiertypes.h \
        ../common/other/widgetgrabber.h \
        mediaplayer.h \
        cimagecacher.h \
        cmediainfoparser.h \
        dialogs.h \
        cinfobar.h \
        ciconprovider.h \
        const_strings.h \
        cpushbutton.h \
        mntres.h \
        cfileopendialog.h \
        csyscommandthread.h \
        treeview.h \
        copypanel.h \
        cfilecopierclient.h \
        cinfomessage.h \
        cprogressbar.h \
        caboutcontent.h \
        csharemounter.h \
        csyscall.h \
        cnativesyscall.h \
        csharemanager.h \
        csharemounterinterface.h \
        cshareeditdialog.h \
        cshareslistdialog.h \
    csharesparser.h \
    cperiodicalprocesscall.h \
    cautoshareswatcher.h \
    bidirectional_subtract.h \
    sharemigration.h \
    dlna.h \
    dlnaservice.h \
    connectionmanager.h\
    avtransport.h


FORMS += \
	QuestionDialog.ui \
	NameDialog.ui \
	LoginPasswordDialog.ui \
    shareslistdialog.ui \
    shareeditdialog.ui

LIBS += -L../ext/lib -lqtstbapi-compl -lqtstbapi -lQxtCore -lQxtGui -lQxtNetwork -lQxtSql -lQxtWeb -lBrisaCore -lBrisaUpnp -lBrisaUtils

RESOURCES += \
	resources.qrc

install.commands = mkdir -p ../../install/$${TARGET};
install.commands += cp lib$${TARGET}.so ../../install/$${TARGET}
install.depends = all
QMAKE_EXTRA_TARGETS += install

TRANSLATIONS += mediaplayer.ts
