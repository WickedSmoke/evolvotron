TEMPLATE = lib
TARGET = evolvotron   # Have to override this or we get "liblibevolvotron"

include (../common.pro)

CONFIG += staticlib

HEADERS += \
        args.h \
	dialog_about.h \
	dialog_favourite.h \
	dialog_functions.h \
	dialog_help.h \
        dialog_mutatable_image_display.h \
	dialog_mutation_parameters.h \
	evolvotron_main.h \
        function_boilerplate.h \
        function_compose_pair.h \
        function_compose_triple.h \
        function_core.h \
        functions.h \
        function_node.h \
        function_node_info.h \
        functions_noise.h \
        function_null.h \
        function_pre_transform.h \
        function_post_transform.h \
        function_registration.h \
        function_registry.h \
        function_top.h \
        function_transform_generalised.h \
	license.h \
        margin.h \
	mutatable_image.h \
	mutatable_image_computer.h \
	mutatable_image_computer_farm.h \
	mutatable_image_computer_task.h \
	mutatable_image_display.h \
	mutatable_image_display_big.h \
	mutation_parameters.h \
        noise.h \
	random.h \
        transform.h \
        transform_factory.h \
	useful.h \
        usage_text.h \
        vbox_scrollview.h \
	xyz.h

SOURCES += \
        args.cpp \
	dialog_about.cpp \
	dialog_help.cpp \
	dialog_favourite.cpp \
	dialog_functions.cpp \
        dialog_mutatable_image_display.cpp \
	dialog_mutation_parameters.cpp \
	evolvotron_main.cpp \
        function_boilerplate.cpp \
        function_compose_pair.cpp \
        function_compose_triple.cpp \
        function_core.cpp \
        functions.cpp \
        function_node.cpp \
        function_node_info.cpp \
        functions_noise.cpp \
        function_null.cpp \
        function_pre_transform.cpp \
        function_post_transform.cpp \
        function_registration.cpp \
        function_registry.cpp \
        function_top.cpp \
        function_transform_generalised.cpp \
	license.cpp \
        margin.cpp \
	mutatable_image.cpp \
	mutatable_image_computer.cpp \
	mutatable_image_computer_farm.cpp \
	mutatable_image_computer_task.cpp \
	mutatable_image_display.cpp \
	mutatable_image_display_big.cpp \
	mutation_parameters.cpp \
        noise.cpp \
	random.cpp \
        transform.cpp \
        transform_factory.cpp \
	useful.cpp \
        vbox_scrollview.cpp \
	xyz.cpp

#########################
# Target to build assembler listings for specific files of interest
# 
asm.target = asm
asm.commands = $(CXX) -S $(CXXFLAGS) $(INCPATH) $<
asm.depends =  functions.cpp
QMAKE_EXTRA_UNIX_TARGETS += asm
