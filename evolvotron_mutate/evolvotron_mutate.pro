TEMPLATE = app

include (../common.pro)

SOURCES += evolvotron_mutate.cpp
DEPENDPATH += ../libevolvotron

TARGETDEPS += ../libevolvotron/libevolvotron.a
LIBS       += ../libevolvotron/libevolvotron.a

QMAKE_CXXFLAGS_RELEASE += -I../libevolvotron
QMAKE_CXXFLAGS_DEBUG   += -I../libevolvotron

target.path = $$INSTALLPATH

INSTALLS += target
