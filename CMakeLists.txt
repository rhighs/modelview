cmake_minimum_required(VERSION 3.22)
project(gltest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER /usr/bin/clang++ CACHE PATH "")

# Build link step
add_executable(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME})

add_subdirectory( "src" )
add_subdirectory( "external" )

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
set(CMAKE_BUILD_TYPE Debug)