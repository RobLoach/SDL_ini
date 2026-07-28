cmake_minimum_required(VERSION 3.16)

# Parse the version out of SDL_ini.h so the header is the single source of truth.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/SDL_ini.h" SDL_INI_VERSION_MAJOR_LINE REGEX "^#define[ \t]+SDL_INI_MAJOR_VERSION[ \t]+[0-9]+$")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/SDL_ini.h" SDL_INI_VERSION_MINOR_LINE REGEX "^#define[ \t]+SDL_INI_MINOR_VERSION[ \t]+[0-9]+$")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/SDL_ini.h" SDL_INI_VERSION_MICRO_LINE REGEX "^#define[ \t]+SDL_INI_MICRO_VERSION[ \t]+[0-9]+$")
string(REGEX REPLACE "^#define[ \t]+SDL_INI_MAJOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL_INI_VERSION_MAJOR "${SDL_INI_VERSION_MAJOR_LINE}")
string(REGEX REPLACE "^#define[ \t]+SDL_INI_MINOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL_INI_VERSION_MINOR "${SDL_INI_VERSION_MINOR_LINE}")
string(REGEX REPLACE "^#define[ \t]+SDL_INI_MICRO_VERSION[ \t]+([0-9]+)$" "\\1" SDL_INI_VERSION_MICRO "${SDL_INI_VERSION_MICRO_LINE}")
set(SDL_INI_VERSION "${SDL_INI_VERSION_MAJOR}.${SDL_INI_VERSION_MINOR}.${SDL_INI_VERSION_MICRO}")
unset(SDL_INI_VERSION_MAJOR_LINE)
unset(SDL_INI_VERSION_MINOR_LINE)
unset(SDL_INI_VERSION_MICRO_LINE)

project(SDL_ini
    VERSION ${SDL_INI_VERSION}
    LANGUAGES C
)

# SDL_ini
add_library(SDL_ini INTERFACE)
target_include_directories(SDL_ini INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Options
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(SDL_INI_IS_MAIN TRUE)

    # Add the cmake package list.
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.cmake")
else()
    set(SDL_INI_IS_MAIN FALSE)
endif()
option(SDL_INI_BUILD_TESTS "Build the SDL_ini test suite" ${SDL_INI_IS_MAIN})

# Tests
if (SDL_INI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
