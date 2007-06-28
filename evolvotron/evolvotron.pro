TEMPLATE = app

include (../common.pro)

PRECOMPILED_HEADER = evolvotron_precompiled.h

SOURCES += evolvotron.cpp

DEPENDPATH += ../libevolvotron ../libfunction
INCLUDEPATH += ../libevolvotron ../libfunction

TARGETDEPS += ../libevolvotron/libevolvotron.a ../libfunction/libfunction.a
LIBS       += ../libevolvotron/libevolvotron.a ../libfunction/libfunction.a


