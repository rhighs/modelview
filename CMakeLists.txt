cmake_minimum_required( VERSION 3.22 )
project( modelview )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

if( APPLE )
    if( NOT CMAKE_OSX_ARCHITECTURES )
        set( CMAKE_OSX_ARCHITECTURES "${CMAKE_HOST_SYSTEM_PROCESSOR}" CACHE STRING "Target macOS architecture" FORCE )
    endif()

    find_program( HOMEBREW_EXECUTABLE brew )
    if( NOT HOMEBREW_EXECUTABLE )
        message( STATUS "Homebrew not found. Install it from https://brew.sh for easy dependency management." )
    else()
        message( STATUS "Homebrew detected. If you need tools, run: brew install cmake ninja" )
    endif()

    message( STATUS "macOS build target architecture: ${CMAKE_OSX_ARCHITECTURES}" )
    message( STATUS "To build universal binaries, configure with -DCMAKE_OSX_ARCHITECTURES=\"arm64;x86_64\"" )
endif()

set( SDL_TEST ON CACHE BOOL "" FORCE )
set( SDL_TESTS ON CACHE BOOL "" FORCE )
set( SDL2_SHARED ON CACHE BOOL "" FORCE )

# Build link step
file( GLOB_RECURSE SOURCES "src/*.cpp" )
file( GLOB_RECURSE HEADERS "src/*.h" )
add_executable( ${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

set_target_properties( ${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME} )

find_package( OpenGL REQUIRED )
target_link_libraries( ${CMAKE_PROJECT_NAME} OpenGL::GL )

add_subdirectory( external/SDL )
target_link_libraries( ${CMAKE_PROJECT_NAME} SDL2::SDL2main SDL2::SDL2-static )

add_subdirectory( external/cglm )
target_link_libraries( ${CMAKE_PROJECT_NAME} cglm )

add_subdirectory( external/glad )
target_link_libraries( ${CMAKE_PROJECT_NAME} glad )

target_include_directories( ${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/external/SDL/include )
target_include_directories( ${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/external/glad/include )
target_include_directories( ${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/external/cglm/include )
target_include_directories( ${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/external/glm )
target_include_directories( ${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/external/stb )

target_compile_definitions( ${PROJECT_NAME} PRIVATE DEV_ENABLED )
if( APPLE )
    target_compile_definitions( ${PROJECT_NAME} PRIVATE GL_SILENCE_DEPRECATION )
endif()

target_compile_options( ${PROJECT_NAME} PRIVATE -Wall )
