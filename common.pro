# Shared definitions included by the other .pro files
# qmake should NOT be executed on this file directly

# append debug or release
CONFIG+= qt thread release

##################
# Set install path.
# For a system-wide install use the next line (and remember to "su" for the "make install" stage)
# INSTALLPATH = /usr/local/bin
# For a personal install use something like the next line

INSTALLPATH = /home/$(USER)/bin

##################
# Improved optimisation options from qmake defaults.
# The README contains some timings showing the (fairly small) effect of changing these.
# 
# Use the next two lines for slight improvement
# Now leaving these ON for general distribution as they DO have SOME effect 
# (or at least they have in the past... see the README), and should be portable.
# NB Switching on -funroll-loops causes some versions of gcc to segv
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -fomit-frame-pointer -ffast-math 

##################
# Architecture specific optimisations
# The following architecture options make the compiled executables less portable.
# (On a different setup you may need to change what is subtracted out of the flags.)
# NB -march=X implies -mcpu=X... no need to specify both the same

contains(CONFIG_OPTS, p4 ){ 
  QMAKE_CXXFLAGS_RELEASE -= -march=i386 -mcpu=i686
  QMAKE_CXXFLAGS_RELEASE += -march=pentium4 -mfpmath=sse -msse2
  BUILD_INFO+= Pentium4
}

contains(CONFIG_OPTS, p3 ){
  QMAKE_CXXFLAGS_RELEASE -= -march=i386 -mcpu=i686
  QMAKE_CXXFLAGS_RELEASE += -march=pentium3 -mfpmath=sse -msse
  BUILD_INFO+= Pentium3  
}

contains(CONFIG_OPTS, xp ){
  QMAKE_CXXFLAGS_RELEASE -= -march=i386 -mcpu=i686
  QMAKE_CXXFLAGS_RELEASE += -march=athlon-xp -mfpmath=sse -msse
  BUILD_INFO+= Athlon-XP
}

##################
# Optimisation insanity
# The next line seems to generate nicer assembler (with better SSE register usage) from some templated code.
# WARNING: gcc grows HUGE (>500MB!!!) and it takes AGES (30mins!!!) with this option.
# Of curiosity value for the hardcore only.  (Untested since templated tuple implemention was dropped).

# QMAKE_CXXFLAGS_RELEASE += -funroll-loops -finline-limit=4000

#######################################
# Version numbering.  VERSION_NUMBER should have been set on the qmake command line (see .configure script)

QMAKE_CXXFLAGS_RELEASE += '-DEVOLVOTRON_VERSION="$$VERSION_NUMBER"'
QMAKE_CXXFLAGS_DEBUG   += '-DEVOLVOTRON_VERSION="$$VERSION_NUMBER"'
QMAKE_CXXFLAGS_RELEASE += '-DEVOLVOTRON_BUILD="$$VERSION_NUMBER (Build options: $$BUILD_INFO Release)"'
QMAKE_CXXFLAGS_DEBUG   += '-DEVOLVOTRON_BUILD="$$VERSION_NUMBER (Build options: $$BUILD_INFO Debug)"'

# qmake's library code can use this too (but only for shared libraries which we don't use)
VERSION=$$VERSION_NUMBER

#######################################
# Disable assertions in release version

QMAKE_CXXFLAGS_RELEASE += -DNDEBUG
QMAKE_CFLAGS_RELEASE += -DNDEBUG

######################################
# Other stuff:
# Disable implicit cast from QString to/from char* - we should be using .local8Bit and ::fromLocal8Bit
# NB We don't use the corresponding -DQT_NO_CAST_ASCII because it breaks QString("...") which is used all over the place
# This probably wouldn't be usable until all the strings were moved out of the app - see Qt I18N docs.
# Also add gcc threading option (not entirely clear whether this is needed but it doesn't seem to hurt)

QMAKE_CXXFLAGS_RELEASE += -DQT_NO_ASCII_CAST -pthread
QMAKE_CXXFLAGS_DEBUG += -DQT_NO_ASCII_CAST -pthread

######################################
# Hide those crufty moc_ files away

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

