TEMPLATE = lib
TARGET = function   # Have to override this or we get "liblibfunction"

include (../common.pro)

CONFIG += staticlib
CONFIG += c++11

HEADERS += $$files(*.h)
SOURCES += $$files(*.cpp)
