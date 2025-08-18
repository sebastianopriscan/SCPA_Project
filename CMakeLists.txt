cmake_minimum_required(VERSION 3.20)
project(SCPA_Project)

include(CTest)
include(versions.cmake)

add_compile_definitions(Wall Wextra _POSIX_C_SOURCE=200809L)

include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(${CMAKE_SOURCE_DIR}/src/include)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(tests)