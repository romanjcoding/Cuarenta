cmake_minimum_required(VERSION 3.16)

project(CuarentaBot LANGUAGES CXX)

add_executable(cuarenta
    main.cpp
    cuarenta.cpp
    bot.cpp
    movegen.cpp
    cli.cpp
    cli_parse.cpp
    cli_render.cpp
)

target_compile_features(cuarenta PRIVATE cxx_std_23)

# If you have headers in an "include/" dir, uncomment:
# target_include_directories(cuarenta PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ---- Warnings (GCC/Clang) ----
if (MSVC)
    target_compile_options(cuarenta PRIVATE /W4 /permissive-)
else()
    target_compile_options(cuarenta PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wformat=2
        -Wundef
        -Wnull-dereference
        -Wdouble-promotion
    )
endif()

if (MSVC)
    target_compile_options(cuarenta PRIVATE
        $<$<CONFIG:Debug>:/Zi>
        $<$<CONFIG:Release>:/O3>
    )
    target_compile_definitions(cuarenta PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
    )
else()
    target_compile_options(cuarenta PRIVATE
        $<$<CONFIG:Debug>:-g;-fno-omit-frame-pointer>
        $<$<CONFIG:Release>:-O3>
    )
    target_compile_definitions(cuarenta PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
    )
endif()

set_target_properties(cuarenta PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
