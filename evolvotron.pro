TEMPLATE = app

# append debug or release
CONFIG+= qt thread release

# Input
HEADERS += \
        args.h \
	dialog_about.h \
	dialog_mutation_parameters.h \
	license.h \
	mutatable_image.h \
	mutatable_image_computer.h \
	mutatable_image_computer_farm.h \
	mutatable_image_computer_task.h \
	mutatable_image_display.h \
	mutation_parameters.h \
	random.h \
	evolvotron_main.h \
	useful.h \
	xyz.h

SOURCES += \
        args.cpp \
	dialog_about.cpp \
	dialog_mutation_parameters.cpp \
	license.cpp \
	mutatable_image.cpp \
	mutatable_image_computer.cpp \
	mutatable_image_computer_farm.cpp \
	mutatable_image_computer_task.cpp \
	mutatable_image_display.cpp \
	mutation_parameters.cpp \
	random.cpp \
	evolvotron.cpp \
	evolvotron_main.cpp \
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
INSTALLS += executable
executable.path = /usr/local/bin
executable.files = evolvotron

#######################################
# Make a .tar.gz
#
tgz.target = evolvotron.tar.gz
tgz.depends = README BUILD LICENSE TODO CHANGES evolvotron.pro doxygen.cfg $$HEADERS $$SOURCES
tgz.commands = tar cvfz evolvotron.tar.gz README BUILD LICENSE TODO CHANGES evolvotron.pro doxygen.cfg $$HEADERS $$SOURCES 
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

