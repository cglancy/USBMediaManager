#-------------------------------------------------
#
# Project created by QtCreator 2013-02-02T10:42:33
#
#-------------------------------------------------

QT       += core gui widgets network concurrent multimedia opengl

TARGET = USBVideoManager
TEMPLATE = app

HEADERS += ./categorynode.h \
    ./library.h \
    ./remotefile.h \
    ./thumbnailfile.h \
    ./utility.h \
    ./video.h \
    ./videofile.h \
    ./aboutdialog.h \
    ./copydialog.h \
    ./copymanager.h \
    ./downloadmanager.h \
    ./flowwidget.h \
    ./inspector.h \
    ./librarylistmodel.h \
    ./mainwindow.h \
    ./medialibrary.h \
    ./metadata.h \
    ./opendialog.h \
    ./preferencesdialog.h \
    ./sortfilterproxymodel.h \
    ./thumbnailmanager.h \
    ./videolistmodel.h
SOURCES += ./aboutdialog.cpp \
    ./categorynode.cpp \
    ./copydialog.cpp \
    ./copymanager.cpp \
    ./downloadmanager.cpp \
    ./flowwidget.cpp \
    ./inspector.cpp \
    ./library.cpp \
    ./librarylistmodel.cpp \
    ./main.cpp \
    ./mainwindow.cpp \
    ./medialibrary.cpp \
    ./metadata.cpp \
    ./opendialog.cpp \
    ./preferencesdialog.cpp \
    ./remotefile.cpp \
    ./sortfilterproxymodel.cpp \
    ./thumbnailfile.cpp \
    ./thumbnailmanager.cpp \
    ./utility.cpp \
    ./video.cpp \
    ./videofile.cpp \
    ./videolistmodel.cpp
FORMS += ./AboutDialog.ui \
    ./CopyDialog.ui \
    ./OpenDialog.ui \
    ./PreferencesDialog.ui \
    ./mainwindow.ui
RESOURCES += resources.qrc

RC_FILE = icon.rc
ICON = video_folder.icns
