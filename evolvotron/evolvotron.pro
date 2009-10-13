TEMPLATE = app

include (../common.pro)

PRECOMPILED_HEADER = evolvotron_precompiled.h

SOURCES += $$system(ls *.cpp)

DEPENDPATH += ../libevolvotron ../libfunction
INCLUDEPATH += ../libevolvotron ../libfunction

TARGETDEPS += ../libevolvotron/libevolvotron.a ../libfunction/libfunction.a
LIBS       += ../libevolvotron/libevolvotron.a ../libfunction/libfunction.a -lboost_program_options


