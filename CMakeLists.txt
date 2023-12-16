cmake_minimum_required(VERSION 3.22)
project(gltest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER /usr/bin/g++ CACHE PATH "")

# Build link step
add_subdirectory( src )
add_subdirectory( external )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
set(CMAKE_BUILD_TYPE Debug)
