set( BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE )

# include_directories(SDL/include
#                     glad/include
#                     stb
#                     cglm/include)

include(${CMAKE_CURRENT_SOURCE_DIR}/SDL/cmake/sdlfind.cmake)

# SDL
# https://github.com/libsdl-org/SDL
set( SDL_LIBC ON CACHE BOOL "" FORCE )
set( SDL_TEST OFF CACHE BOOL "" FORCE )
add_subdirectory( SDL )
find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
find_library(SDL2_LIBRARY NAME SDL2)
include_directories(${SDL2_INCLUDE_DIRS})

target_link_libraries( ${CMAKE_PROJECT_NAME} SDL2::SDL2 SDL2::SDL2main SDL2::SDL2-static )

target_include_directories( ${CMAKE_PROJECT_NAME} SYSTEM PRIVATE "glad" )
add_library(glad STATIC glad/src/glad.c)

target_include_directories( ${CMAKE_PROJECT_NAME} SYSTEM PRIVATE "cglm" )

target_include_directories( ${CMAKE_PROJECT_NAME} SYSTEM PRIVATE "stb" )

target_include_directories( ${CMAKE_PROJECT_NAME} SYSTEM PRIVATE "glm" )
