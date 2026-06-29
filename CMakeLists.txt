cmake_minimum_required(VERSION 3.16)
project(SDL_ini
    VERSION 1.0.0
    LANGUAGES C
)

# SDL_ini
add_library(SDL_ini INTERFACE)
target_include_directories(SDL_ini INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Options
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(SDL_INI_IS_MAIN TRUE)
else()
    set(SDL_INI_IS_MAIN FALSE)
endif()
option(SDL_INI_BUILD_TESTS "Build the SDL_ini test suite" ${SDL_INI_IS_MAIN})

# Tests
if(SDL_INI_BUILD_TESTS)
    find_package(SDL3 REQUIRED)
    add_executable(SDL_ini_test test/SDL_ini_test.c)
    target_link_libraries(SDL_ini_test PRIVATE SDL_ini SDL3::SDL3)
    set_target_properties(SDL_ini_test PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
    )
    enable_testing()
    add_test(NAME SDL_ini_tests COMMAND SDL_ini_test)
endif()
