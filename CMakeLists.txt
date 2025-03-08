cmake_minimum_required(VERSION 3.20)
project(SCPA_Project)

include(CTest)

add_compile_definitions(Wall Wextra)

include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)

if(DEFINED CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
   add_compile_definitions(TEST_BUILD) 
endif(DEFINED CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "Debug")


add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(tests)