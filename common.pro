# Shared definitions included by the other .pro files
# qmake should NOT be executed on this file directly

# append debug or release
CONFIG+= qt thread stl exceptions release       #release  #debug
QT += xml

#######################################
# Control platform specific code
# (the Qt platform scopes don't seem that useful; might be easier to just add DEFINES+=... explicitly 

unix {
  DEFINES+=PLATFORM_LINUX      # of course PLATFORM_BSD is more appropriate to some unices
}

macx {
# NB This doesn't actually seem to be selected on Fink builds 
# (presumably it applies to Quartz builds instead)
# Fink does seem to have a useful __DARWIN_X11__ define though,
# which is used to set PLATFORM_BSD in libevolvotron/platform_specific.cpp
  DEFINES+=PLATFORM_BSD
}

win32 {
# You might want to add some appropriate code to libevolvotron/platform_specific.cpp
# and set a #define here to select it
}

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
# Used to use -pthread -DBOOST_SP_USE_PTHREADS as a workround for debian bug 485434 (maybe only needed on sparc?)
# but it seemed to cause problems with app not quitting once boost::program_options started being used.

QMAKE_CXXFLAGS_RELEASE += -DQT_NO_ASCII_CAST
QMAKE_CXXFLAGS_DEBUG += -DQT_NO_ASCII_CAST

######################################
# Hide those crufty moc_ files away

MOC_DIR = moc

##################
# OPTION: Enable these options for profiling
#
#QMAKE_LFLAGS_RELEASE += -pg
#QMAKE_CXXFLAGS_RELEASE += -pg
