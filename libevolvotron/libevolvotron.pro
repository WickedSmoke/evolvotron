TEMPLATE = lib
TARGET = evolvotron   # Have to override this or we get "liblibevolvotron"

include (../common.pro)

CONFIG += staticlib

HEADERS += \
        args.h \
	dialog_about.h \
	dialog_mutation_parameters.h \
	evolvotron_main.h \
        function_boilerplate.h \
        functions.h \
        function_node.h \
        function_node_info.h \
        function_pre_transform.h \
        function_post_transform.h \
        function_registration.h \
        function_registry.h \
	license.h \
        margin.h \
        matrix.h \
	mutatable_image.h \
	mutatable_image_computer.h \
	mutatable_image_computer_farm.h \
	mutatable_image_computer_task.h \
	mutatable_image_display.h \
	mutation_parameters.h \
        noise.h \
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
	evolvotron_main.cpp \
        function_boilerplate.cpp \
        functions.cpp \
        function_node.cpp \
        function_node_info.cpp \
        function_pre_transform.cpp \
        function_post_transform.cpp \
        function_registration.cpp \
        function_registry.cpp \
	license.cpp \
        margin.cpp \
        matrix.cpp \
	mutatable_image.cpp \
	mutatable_image_computer.cpp \
	mutatable_image_computer_farm.cpp \
	mutatable_image_computer_task.cpp \
	mutatable_image_display.cpp \
	mutation_parameters.cpp \
        noise.cpp \
	random.cpp \
        transform.cpp \
        transform_factory.cpp \
        tuple.cpp \
	useful.cpp \
	xyz.cpp


#########################
# Target to build assembler listings for specific files of interest
# 
asm.target = asm
asm.commands = $(CXX) -S $(CXXFLAGS) $(INCPATH) $<
asm.depends =  functions.cpp
QMAKE_EXTRA_UNIX_TARGETS += asm
