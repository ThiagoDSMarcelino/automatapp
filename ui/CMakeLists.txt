find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()
qt_add_executable(automatapp main.cpp
    main_window/main_window.h main_window/main_window.cpp main_window/main_window.ui
    alphabet_dialog/alphabet_dialog.h alphabet_dialog/alphabet_dialog.cpp alphabet_dialog/alphabet_dialog.ui
    diagram/diagram_scene.h diagram/diagram_scene.cpp
    diagram/diagram_view.h diagram/diagram_view.cpp
    diagram/state_item.h diagram/state_item.cpp
    diagram/edge_item.h diagram/edge_item.cpp)

target_include_directories(automatapp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(automatapp PRIVATE automata_core Qt6::Widgets)
target_compile_options(automatapp PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
