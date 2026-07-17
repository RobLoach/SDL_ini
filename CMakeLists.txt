cmake_minimum_required(VERSION 3.16)
project(SDL_ini
    VERSION 1.2.0
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
