#-------------------------------------------------
#
# Project created by QtCreator 2017-01-12T14:51:28
#
#-------------------------------------------------

QT       += core gui opengl xml

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = lmpick
TEMPLATE = app
INCLUDEPATH += $$PWD/src

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += src/main.cc\
        src/mainwindow.cc \
    src/mainopenglview.cc \
    src/octaeder.cc \
    src/globalscenestate.cc \
    src/drawable.cc \
    src/volumedata.cc \
    src/landmark.cc \
    src/camera.cc \
    src/landmarklist.cc \
    src/sphere.cc \
    src/landmarklistpainter.cc \
    src/renderingthread.cc \
    src/landmarklistio.cc \
    src/landmarktablemodel.cc \
    src/landmarktableview.cc \
    src/qruntimeexeption.cc \
    src/aboutdialog.cc


HEADERS  += src/mainwindow.hh \
    src/mainopenglview.hh \
    src/octaeder.hh \
    src/globalscenestate.hh \
    src/drawable.hh \
    src/volumedata.hh \
    src/landmark.hh \
    src/camera.hh \
    src/landmarklist.hh \
    src/sphere.hh \
    src/landmarklistpainter.hh \
    src/renderingthread.hh \
    src/landmarklistio.hh \
    src/errormacro.hh \
    src/landmarktablemodel.hh \
    src/landmarktableview.hh \
    src/qruntimeexeption.hh \
    src/aboutdialog.hh

FORMS    += mainwindow.ui \
    src/aboutdialog.ui

DISTFILES += \
    shaders/view.glsl \
    shaders/basic_frag.glsl \
    README.md \
    shaders_120/volume_2nd_pass_vtx.glsl \
    shaders_120/volume_1st_pass_frag.glsl \
    shaders_120/volume_1st_pass_vtx.glsl \
    shaders_120/volume_2nd_pass_frag.glsl \
    shaders_120/volume_blit_frag.glsl \
    shaders_120/shere_vtx.glsl \
    shaders_330/volume_2nd_pass_vtx.glsl \
    shaders_330/volume_1st_pass_frag.glsl \
    shaders_330/volume_1st_pass_vtx.glsl \
    shaders_330/volume_2nd_pass_frag.glsl \
    shaders_330/volume_blit_frag.glsl \
    shaders_330/shere_vtx.glsl \
    src/icons/auto_snapshot.png \
    src/icons/auto_snapshot_on.png \
    src/icons/document-open-volume.png \
    src/icons/document-open.png \
    src/icons/document-save-as.png \
    src/icons/document-save.png \
    src/icons/snapshot.png

RESOURCES += \
    lmpick.qrc

CONFIG += link_pkgconfig
PKGCONFIG += miamesh-2.4


