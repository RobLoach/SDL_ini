# SDL_ini_test
find_package(SDL3 REQUIRED)

add_executable(SDL_ini_test SDL_ini_test.c)
target_link_libraries(SDL_ini_test PRIVATE SDL_ini SDL3::SDL3 SDL3::SDL3_test)
set_target_properties(SDL_ini_test PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)
target_compile_options(SDL_ini_test PRIVATE
    $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<C_COMPILER_ID:MSVC>:/W4 /WX>
)
add_test(NAME SDL_ini_tests COMMAND SDL_ini_test)
