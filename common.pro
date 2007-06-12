
# Shared definitions included by the other .pro files
# qmake should NOT be executed on this file directly

# append debug or release
CONFIG+= qt thread stl exceptions release       #release  #debug

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
#QMAKE_LFLAGS_RELEASE += -pg
#QMAKE_CXXFLAGS_RELEASE += -pg
