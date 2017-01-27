#-------------------------------------------------
#
# Project created by QtCreator 2017-01-12T14:51:28
#
#-------------------------------------------------

QT       += core gui opengl xml

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = lmpick
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cc\
        mainwindow.cc \
    mainopenglview.cc \
    octaeder.cc \
    globalscenestate.cc \
    drawable.cc \
    volumedata.cc \
    landmark.cc \
    camera.cc \
    landmarklist.cc \
    sphere.cc \
    landmarklistpainter.cc \
    renderingthread.cc \
    landmarklistio.cc \
    landmarktablemodel.cc \
    landmarkltableview.cc


HEADERS  += mainwindow.hh \
    mainopenglview.hh \
    octaeder.hh \
    globalscenestate.hh \
    drawable.hh \
    volumedata.hh \
    landmark.hh \
    camera.hh \
    landmarklist.hh \
    sphere.hh \
    landmarklistpainter.hh \
    renderingthread.hh \
    landmarklistio.hh \
    errormacro.hh \
    landmarktablemodel.hh \
    landmarkltableview.hh

FORMS    += mainwindow.ui

DISTFILES += \
    shaders/view.glsl \
    shaders/basic_frag.glsl \
    README.md \
    shaders/volume_2nd_pass_vtx.glsl \
    shaders/volume_1st_pass_frag.glsl \
    shaders/volume_1st_pass_vtx.glsl \
    shaders/volume_2nd_pass_frag.glsl \
    shaders/volume_blit_frag.glsl \
    shaders/shere_vtx.glsl \
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


