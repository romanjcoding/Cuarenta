cmake_minimum_required(VERSION 3.16)

project(PropositionalLogic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(propositional_logic
    main.cpp
    cuarenta.cpp
    unit_tests.cpp
)

# target_include_directories(propositional_logic
#     PUBLIC
#         ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

if (MSVC)
    target_compile_options(propositional_logic PRIVATE /O2)
else()
    target_compile_options(propositional_logic PRIVATE -O3)
endif()