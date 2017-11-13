TEMPLATE = lib
TARGET = function   # Have to override this or we get "liblibfunction"

include (../common.pro)

CONFIG += staticlib

HEADERS += $$files(*.h)
SOURCES += $$files(*.cpp)
