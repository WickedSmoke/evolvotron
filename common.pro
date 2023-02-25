# Shared definitions included by the other .pro files
# qmake should NOT be executed on this file directly

# append debug or release
CONFIG+= qt thread stl exceptions release       #release  #debug
# QT +=

#######################################
# Version numbering.  VERSION_NUMBER should have been set on the qmake command line (see .configure script)

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
# Used to use -pthread -DBOOST_SP_USE_PTHREADS as a workround for debian bug 485434 (maybe only needed on sparc?)
# but it seemed to cause problems with app not quitting once boost::program_options started being used.

QMAKE_CXXFLAGS_RELEASE += -DQT_NO_ASCII_CAST
QMAKE_CXXFLAGS_DEBUG += -DQT_NO_ASCII_CAST

######################################
# Hide intermediate build files away

MOC_DIR = moc
RCC_DIR = moc
OBJECTS_DIR = ../obj

##################
# OPTION: Enable these options for profiling
#
#QMAKE_LFLAGS_RELEASE += -pg
#QMAKE_CXXFLAGS_RELEASE += -pg
