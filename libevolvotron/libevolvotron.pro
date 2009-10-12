TEMPLATE = lib
TARGET = evolvotron   # Have to override this or we get "liblibevolvotron"

include (../common.pro)

CONFIG += staticlib

PRECOMPILED_HEADER = libevolvotron_precompiled.h

INCLUDEPATH += ../libfunction
DEPENDPATH += ../libfunction

HEADERS += $$system(ls *.h)
SOURCES += $$system(ls *.cpp)
