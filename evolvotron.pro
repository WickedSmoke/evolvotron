TEMPLATE = app

# append debug or release
CONFIG+= qt thread release

##################
# Better optimisations than qmake defaults IF you have the right processor.
# The -mfpmath=sse -msse2 options (apparently NOT implied by -march alone) 
# seem to be needed to generate SSE instructions on the authors setup.
# The larger inline limit helps with template generated code.
# There is a nice summary of gcc optimisation options at http://freshmeat.net/articles/view/730/
#
# Uncomment BOTH of the next two lines on a P4 system:
#QMAKE_CXXFLAGS_RELEASE -= -march=i386 -O2
#QMAKE_CXXFLAGS_RELEASE += -march=pentium4 -mfpmath=sse -msse2 -O3 -ffast-math -funroll-loops -finline-limit=4000 -fomit-frame-pointer
#
# On a P3 try -msse instead of -msse2 ?

# Input
HEADERS += \
        args.h \
	dialog_about.h \
	dialog_mutation_parameters.h \
	evolvotron_main.h \
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
# This doesn't work on RedHat... tries to strip install directory, not file
executable.path = /usr/local/bin
executable.files = evolvotron
INSTALLS += executable

#######################################
# Make a .tar.gz
# This is build from the parent directory so it will unpack tidily
TGZ_FILES = README BUILD LICENSE TODO CHANGES configure evolvotron.pro doxygen.cfg $$HEADERS $$SOURCES
TGZ_FILES_PREFIXED = $$join(TGZ_FILES," evolvotron/","evolvotron/") 
tgz.target = evolvotron.tar.gz
tgz.depends = $$TGZ_FILES
tgz.commands = cd .. ; tar cvfz evolvotron/evolvotron.tar.gz $$TGZ_FILES_PREFIXED
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

#Unfortunately it doesn't generate rm -r -f doc, so this doesn't work
#QMAKE_CLEAN += doc

# Separate target for cleaning up documentation
QMAKE_EXTRA_UNIX_TARGETS += realclean
realclean.depends = clean
realclean.commands = rm -r -f doc

######################################
# Hide those crufty moc_ files away
#
MOC_DIR = moc

##################
# Use these for profiling
#
#QMAKE_CXXFLAGS_RELEASE -= -fomit-frame-pointer
#QMAKE_LFLAGS_RELEASE += -pg
#QMAKE_CXXFLAGS_RELEASE += -pg
