TEMPLATE = lib
TARGET = function   # Have to override this or we get "liblibfunction"

include (../common.pro)

CONFIG += staticlib

PRECOMPILED_HEADER = libfunction_precompiled.h

HEADERS += $$system(ls *.h)
SOURCES += $$system(ls *.cpp)

#########################
# Target to build assembler listings for specific files of interest
# 
asm.target = asm
asm.commands = $(CXX) -S $(CXXFLAGS) $(INCPATH) $^
asm.depends = functions_friezegroup_jump.cpp functions_friezegroup_hop.cpp
QMAKE_EXTRA_UNIX_TARGETS += asm
