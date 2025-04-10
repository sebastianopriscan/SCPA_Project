cmake_minimum_required(VERSION 3.20)
project(SCPA_Project)

include(CTest)

add_compile_definitions(Wall Wextra)

include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(${CMAKE_SOURCE_DIR}/src/include)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(tests)