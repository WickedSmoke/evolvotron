TEMPLATE = lib
TARGET = evolvotron   # Have to override this or we get "liblibevolvotron"

include (../common.pro)

CONFIG += staticlib

PRECOMPILED_HEADER = libevolvotron_precompiled.h

INCLUDEPATH += ../libfunction

HEADERS += \
        args.h \
	dialog_about.h \
	dialog_favourite.h \
	dialog_functions.h \
	dialog_help.h \
        dialog_mutatable_image_display.h \
	dialog_mutation_parameters.h \
	evolvotron_main.h \
	license.h \
	mutatable_image.h \
	mutatable_image_computer.h \
	mutatable_image_computer_farm.h \
	mutatable_image_computer_task.h \
	mutatable_image_display.h \
	mutatable_image_display_big.h \
        mutation_parameters_qobject.h \
        transform_factory.h \
        usage_text.h \
        vbox_scrollview.h

SOURCES += \
        args.cpp \
	dialog_about.cpp \
	dialog_help.cpp \
	dialog_favourite.cpp \
	dialog_functions.cpp \
        dialog_mutatable_image_display.cpp \
	dialog_mutation_parameters.cpp \
	evolvotron_main.cpp \
	license.cpp \
	mutatable_image.cpp \
	mutatable_image_computer.cpp \
	mutatable_image_computer_farm.cpp \
	mutatable_image_computer_task.cpp \
	mutatable_image_display.cpp \
	mutatable_image_display_big.cpp \
        mutation_parameters_qobject.cpp \
        transform_factory.cpp \
        vbox_scrollview.cpp

