#-------------------------------------------------
#
# Project created by QtCreator 2014-11-16T06:57:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutomaticTrackingCamera
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    DSCP.cpp \
    ObjectTracking.cpp \
    PTZcontrol.cpp \
    rs232.c \
    MotionDetection.cpp

HEADERS  += mainwindow.h \
    DSCP.h \
    ObjectTracking.h \
    PTZcontrol.h \
    rs232.h \
    MotionDetection.h

FORMS    += mainwindow.ui

RESOURCES += \
    resourcefile.qrc

INCLUDEPATH += D:\tool\opencvResult\openCV\include \
               D:\tool\opencvResult\openCV\include\openCV \
               D:\tool\opencvResult\openCV\include\openCV2

LIBS +=   D:\tool\opencvResult\openCV\bin\libopencv_calib3d249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_contrib249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_core249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_features2d249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_flann249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_gpu249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_highgui249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_imgproc249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_legacy249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_ml249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_nonfree249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_objdetect249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_ocl249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_photo249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_stitching249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_superres249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_video249.dll \
          D:\tool\opencvResult\openCV\bin\libopencv_videostab249.dll \
          D:\tool\opencvResult\openCV\bin\opencv_ffmpeg249.dll
