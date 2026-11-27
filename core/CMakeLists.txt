include(FetchContent)

FetchContent_Declare(json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
  URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(automata_core STATIC ${CORE_SOURCES})

target_include_directories(automata_core PUBLIC include)
target_link_libraries(automata_core PRIVATE nlohmann_json::nlohmann_json)
target_compile_options(automata_core PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
