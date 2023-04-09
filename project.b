sources_cpp: func [path /local it] [
    files: read path
    remove-each it files [ne? %.cpp skip tail it -4]
    sources_from path files
]

application: does [
    include_from [%libfunction %libevolvotron]
    libs_from %. [%evolvotron %function]
    unix  [libs %boost_program_options]
    win32 [libs %boost_program_options-x64]
    qt [widgets]
]

lib %function [
    include_from %libfunction
    sources_cpp %libfunction
]

lib %evolvotron [
    qt/no-link [widgets]
    include_from [%libfunction %libevolvotron]
    sources_cpp %libevolvotron
]

exe %evolv [
    application
    sources [%evolvotron/icons.qrc]
    sources_cpp %evolvotron
    win32 [sources [%dist/app.rc]]
]

exe %evolv_mutate [
    application
    sources_cpp %evolvotron_mutate
]

exe %evolv_render [
    application
    sources_cpp %evolvotron_render
]
