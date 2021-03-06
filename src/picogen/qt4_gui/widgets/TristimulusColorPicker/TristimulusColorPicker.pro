# -------------------------------------------------
# Project created by QtCreator 2010-03-29T15:16:26
# -------------------------------------------------
TARGET = TristimulusColorPicker
TEMPLATE = app
SOURCES += main.cc \
    mainwindow.cc \
    tristimuluscolorpicker.cc
HEADERS += mainwindow.hh \
    tristimuluscolorpicker.hh
FORMS += mainwindow.ui \
    tristimuluscolorpicker.ui
include(../../../upstream/QtSolutions/qtcolortriangle-2.6_1-opensource/src/qtcolortriangle.pri)

OTHER_FILES += tristimuluscolorpicker.pri

LIBS += ../../../.libs/libredshift.a
INCLUDEPATH += ../../../renderer ../../../auxiliary
DEFINES += __STDC_LIMIT_MACROS
