TEMPLATE = app

include (../common.pro)

SOURCES += evolvotron_render.cpp
DEPENDPATH += ../libevolvotron

LIBS += ../libevolvotron/libevolvotron.a

QMAKE_CXXFLAGS_RELEASE += -I../libevolvotron
QMAKE_CXXFLAGS_DEBUG   += -I../libevolvotron

target.path = $$INSTALLPATH

INSTALLS += target
