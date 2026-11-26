find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()
qt_add_executable(automatapp main.cpp
    main_window.h main_window.cpp main_window.ui)

target_link_libraries(automatapp PRIVATE automata_core Qt6::Widgets)
target_compile_options(automatapp PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
