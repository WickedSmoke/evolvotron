TEMPLATE = app

QT += widgets
  
include (../common.pro)

SOURCES += $$files(*.cpp)

DEPENDPATH += ../libevolvotron ../libfunction
INCLUDEPATH += ../libevolvotron ../libfunction

TARGETDEPS += ../libevolvotron/libevolvotron.a ../libfunction/libfunction.a
LIBS       += ../libevolvotron/libevolvotron.a ../libfunction/libfunction.a -lboost_program_options


