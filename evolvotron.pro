TEMPLATE = app

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
# Use the next two lines IF you have the right processor (P4)
QMAKE_CXXFLAGS_RELEASE -= -march=i386
QMAKE_CXXFLAGS_RELEASE += -march=pentium4 -mfpmath=sse -msse2
#
# Use the next line for a "super-release" final build only.
# It takes AGES and gcc grows HUGE (~100MB), so not too good for development.
# But it does do a MUCH better job with template code, especially if SSE code is enabled
#QMAKE_CXXFLAGS_RELEASE += -funroll-loops -finline-limit=4000

# Input
HEADERS += \
        args.h \
	dialog_about.h \
	dialog_mutation_parameters.h \
	evolvotron_main.h \
        function.h \
        functions.h \
        function_node.h \
        function_node_using.h \
        function_pre_transform.h \
        function_post_transform.h \
        function_registration.h \
        function_registry.h \
	license.h \
        matrix.h \
	mutatable_image.h \
	mutatable_image_computer.h \
	mutatable_image_computer_farm.h \
	mutatable_image_computer_task.h \
	mutatable_image_display.h \
	mutation_parameters.h \
	random.h \
        transform.h \
        transform_factory.h \
        tuple.h \
	useful.h \
	xyz.h

SOURCES += \
        args.cpp \
	dialog_about.cpp \
	dialog_mutation_parameters.cpp \
	evolvotron.cpp \
	evolvotron_main.cpp \
        function.cpp \
        functions.cpp \
        function_node.cpp \
        function_node_using.cpp \
        function_pre_transform.cpp \
        function_post_transform.cpp \
        function_registration.cpp \
        function_registry.cpp \
	license.cpp \
        matrix.cpp \
	mutatable_image.cpp \
	mutatable_image_computer.cpp \
	mutatable_image_computer_farm.cpp \
	mutatable_image_computer_task.cpp \
	mutatable_image_display.cpp \
	mutation_parameters.cpp \
	random.cpp \
        transform.cpp \
        transform_factory.cpp \
        tuple.cpp \
	useful.cpp \
	xyz.cpp


#######################################
# Version numbering.  This is ENTIRELY controlled by what is echoed by the VERSION script
#
VERSION_NUMBER = $$system(./VERSION)
QMAKE_CXXFLAGS_RELEASE += '-DEVOLVOTRON_VERSION_STRING="$$VERSION_NUMBER (release build)"'
QMAKE_CXXFLAGS_DEBUG   += '-DEVOLVOTRON_VERSION_STRING="$$VERSION_NUMBER (debug build)"'

#######################################
# Disable assertions in release version
#
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG
QMAKE_CFLAGS_RELEASE += -DNDEBUG

######################################
# Threading option needed ?
#
QMAKE_CXXFLAGS_RELEASE += -pthread
QMAKE_CXXFLAGS_DEBUG += -pthread

#######################################
# Install targets
#
# This doesnt actually work on RedHat... tries to strip install directory, not file
executable.path = /usr/local/bin
executable.files = evolvotron
INSTALLS += executable

#######################################
# Make a .tar.gz
# This is build from the parent directory so it will unpack tidily
TGZ_FILES = README BUILD LICENSE TODO CHANGES VERSION configure evolvotron.pro doxygen.cfg $$HEADERS $$SOURCES
TGZ_FILES_PREFIXED = $$join(TGZ_FILES," evolvotron/","evolvotron/") 
tgz.target = evolvotron.tar.gz
tgz.depends = $$TGZ_FILES
tgz.commands = cd .. ; tar cvfz evolvotron/evolvotron.tar.gz $$TGZ_FILES_PREFIXED ; echo "*** Suggestion: mv evolvotron.tar.gz evolvotron-$${VERSION_NUMBER}.tar.gz"
QMAKE_EXTRA_UNIX_TARGETS += tgz

#####################################
# Stuff to drive doxygen
#
doc.target = doc
doc.commands = mkdir -p doc ; doxygen doxygen.cfg
doc.depends = $$HEADERS $$SOURCES
QMAKE_EXTRA_UNIX_TARGETS += doc

#Add this to build documentation every time
#TARGETDEPS += $$doc.target

#Unfortunately it doesnt generate rm -r -f doc, so this doesnt work
#QMAKE_CLEAN += doc

# Separate target for cleaning up documentation
QMAKE_EXTRA_UNIX_TARGETS += realclean
realclean.depends = clean
realclean.commands = rm -r -f doc

#########################
# Target to build assembler listings for specific files of interest
# 
asm.target = asm
asm.commands = $(CXX) -S $(CXXFLAGS) $(INCPATH) $<
asm.depends = function.cpp
QMAKE_EXTRA_UNIX_TARGETS += asm

######################################
# Hide those crufty moc_ files away
#
MOC_DIR = moc

##################
# OPTION: Use these options for profiling
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
