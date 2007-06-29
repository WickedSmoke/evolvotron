TEMPLATE = app

include (../common.pro)

PRECOMPILED_HEADER = evolvotron_match_precompiled.h

SOURCES += evolvotron_match.cpp

DEPENDPATH += ../libevolvotron ../libfunction
INCLUDEPATH += ../libevolvotron ../libfunction

TARGETDEPS += ../libevolvotron/libevolvotron.a ../libfunction/libfunction.a
LIBS       += ../libevolvotron/libevolvotron.a ../libfunction/libfunction.a
