file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(automata_core STATIC ${CORE_SOURCES})

target_include_directories(automata_core PUBLIC include)
target_compile_options(automata_core PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
