cmake_minimum_required( VERSION 3.22 )
project( modelview )


set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )
set( CMAKE_CXX_COMPILER /usr/bin/clang++ CACHE PATH "" )

# define dev debug symbol
if (WIN32)
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /DDEV_ENABLED" )
else (WIN32)
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEV_ENABLED" )
endif (WIN32)

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
target_link_libraries( ${CMAKE_PROJECT_NAME} ${OPENGL_LIBRARIES} )

add_subdirectory( external/SDL )
target_link_libraries( ${CMAKE_PROJECT_NAME} SDL2::SDL2main SDL2::SDL2-static )

add_subdirectory( external/cglm )
target_link_libraries( ${CMAKE_PROJECT_NAME} cglm )

add_subdirectory( external/glad )
target_link_libraries( ${CMAKE_PROJECT_NAME} glad )

target_include_directories( ${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/external/glad/include )
target_include_directories( ${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/external/cglm/include )
target_include_directories( ${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/external/stb )

target_compile_options( ${PROJECT_NAME} PRIVATE -Wall )
set( CMAKE_BUILD_TYPE Release )
