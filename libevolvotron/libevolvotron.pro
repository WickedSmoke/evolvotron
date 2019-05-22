TEMPLATE = lib

QT += widgets

TARGET = evolvotron   # Have to override this or we get "liblibevolvotron"

include (../common.pro)

CONFIG += staticlib
CONFIG += c++11

INCLUDEPATH += ../libfunction
DEPENDPATH += ../libfunction

HEADERS += $$files(*.h)
SOURCES += $$files(*.cpp)
