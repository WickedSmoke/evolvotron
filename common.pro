# Shared definitions included by the other .pro files
# qmake should NOT be executed on this file directly

# append debug or debug
CONFIG+= qt thread release

##################
# Improved optimisation options from qmake defaults
# (There is a nice summary of gcc optimisation options at http://freshmeat.net/articles/view/730/ )
# 
# Use the next two lines to generally improve things
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -ffast-math -fomit-frame-pointer 
#
# Use the next two lines ONLY IF you have the right processor (P4)
#QMAKE_CXXFLAGS_RELEASE -= -march=i386
#QMAKE_CXXFLAGS_RELEASE += -march=pentium4 -mfpmath=sse -msse2
#
# Here's the same thing (UNTESTED) for P3 
#QMAKE_CXXFLAGS_RELEASE -= -march=i386
#QMAKE_CXXFLAGS_RELEASE += -march=pentium3 -mfpmath=sse -msse
#
# Here's the same thing (UNTESTED) for Athalon-XP
#QMAKE_CXXFLAGS_RELEASE -= -march=i386
#QMAKE_CXXFLAGS_RELEASE += -march=athalon-xp -mfpmath=sse -msse
#
#
# Use the next line for a "super-release" final build only.
# It takes AGES and gcc grows HUGE (~100MB), so not too good for development.
# But it does do a MUCH better job with template code, especially if SSE code is enabled
#QMAKE_CXXFLAGS_RELEASE += -funroll-loops 
#QMAKE_CXXFLAGS_RELEASE += -finline-limit=4000

##################
# Set install path.
# For a system-wide install use the next line (and remember to "su" for the "make install" stage)
# INSTALLPATH = /usr/local/bin
# For a personal install use something like the next line
INSTALLPATH = /home/$(USER)/bin

#######################################
# Version numbering.  This is ENTIRELY controlled by what is echoed by the VERSION script
#
VERSION_NUMBER = $$system(../VERSION)
QMAKE_CXXFLAGS_RELEASE += '-DEVOLVOTRON_VERSION="$$VERSION_NUMBER"'
QMAKE_CXXFLAGS_DEBUG   += '-DEVOLVOTRON_VERSION="$$VERSION_NUMBER"'
QMAKE_CXXFLAGS_RELEASE += '-DEVOLVOTRON_BUILD="$$VERSION_NUMBER (release build)"'
QMAKE_CXXFLAGS_DEBUG   += '-DEVOLVOTRON_BUILD="$$VERSION_NUMBER (debug build)"'

# qmake's library code can use this too (but only for shared libraries which we don't use)
VERSION=$$VERSION_NUMBER

#######################################
# Disable assertions in release version
#
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG
QMAKE_CFLAGS_RELEASE += -DNDEBUG

######################################
# Other stuff:
# Disable implicit cast from QString to char*
# Threading option (not entirely clear this is needed but it doesn't seem to hurt)

QMAKE_CXXFLAGS_RELEASE += -DQT_NO_ASCII_CAST -pthread
QMAKE_CXXFLAGS_DEBUG += -DQT_NO_ASCII_CAST -pthread

######################################
# Hide those crufty moc_ files away
#
MOC_DIR = moc

##################
# OPTION: Enable these options for profiling
#
#QMAKE_CXXFLAGS_RELEASE -= -fomit-frame-pointer
#QMAKE_LFLAGS_RELEASE += -pg
#QMAKE_CXXFLAGS_RELEASE += -pg

###################
# OPTION: Use this to link vs ccmalloc library
# (This hasn't been particularly useful though;
# the only memory leaks so far have been related
# to inter-thread comms and timing dependent.
# Linking vs ccmalloc (or using vgrind) changed timings
# and caused the leak to disappear.)
#
#QMAKE_LINK=ccmalloc g++

