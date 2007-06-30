TEMPLATE = lib
TARGET = function   # Have to override this or we get "liblibfunction"

include (../common.pro)

CONFIG += staticlib

PRECOMPILED_HEADER = libfunction_precompiled.h

HEADERS += \
        function_boilerplate.h \
        function_boilerplate_instantiate.h \
        function_compose_pair.h \
        function_compose_triple.h \
        function_constant.h \
        function_identity.h \
        function_node.h \
        function_node_info.h \
        function_pre_transform.h \
        function_post_transform.h \
        function_registration.h \
        function_registry.h \
        function_top.h \
        function_transform.h \
        function_transform_generalised.h \
        functions_arithmetic.h \
        functions_choose.h \
        functions_filter.h \
        functions_friezegroup_hop.h \
        functions_friezegroup_spinhop.h \
        functions_geometry.h \
        functions_gradient.h \
        functions_juliabrot.h \
        functions_kaleidoscope.h \
        functions_magnitude.h \
        functions_misc.h \
        functions_noise.h \
        functions_render.h \
        functions_shadow.h \
        functions_spherical.h \
        functions_spiral.h \
        functions_transform.h \
        margin.h \
	mutation_parameters.h \
        noise.h \
	random.h \
        register_all_functions.h \
        transform.h \
	useful.h \
	xyz.h

SOURCES += \
        function_boilerplate.cpp \
        function_compose_pair.cpp \
        function_compose_triple.cpp \
        function_constant.h \
        function_identity.h \
        function_node.cpp \
        function_node_info.cpp \
        function_pre_transform.cpp \
        function_post_transform.cpp \
        function_registration.cpp \
        function_registry.cpp \
        function_top.cpp \
        function_transform.cpp \
        function_transform_generalised.cpp \
        functions_arithmetic.cpp \
        functions_choose.cpp \
        functions_filter.cpp \
        functions_friezegroup_hop.cpp \
        functions_friezegroup_spinhop.cpp \
        functions_geometry.cpp \
        functions_gradient.cpp \
        functions_juliabrot.cpp \
        functions_kaleidoscope.cpp \
        functions_magnitude.cpp \
        functions_misc.cpp \
        functions_noise.cpp \
        functions_render.cpp \
        functions_shadow.cpp \
        functions_spherical.cpp \
        functions_spiral.cpp \
        functions_transform.cpp \
        margin.cpp \
	mutation_parameters.cpp \
        noise.cpp \
	random.cpp \
        register_all_functions.cpp \
        transform.cpp \
	useful.cpp \
	xyz.cpp

#########################
# Target to build assembler listings for specific files of interest
# 
asm.target = asm
asm.commands = $(CXX) -S $(CXXFLAGS) $(INCPATH) $<
asm.depends =  functions_magnitude.cpp
QMAKE_EXTRA_UNIX_TARGETS += asm
