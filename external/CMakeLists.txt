# SDL
# set( BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE )
# https://github.com/libsdl-org/SDL
# set( SDL_LIBC ON CACHE BOOL "" FORCE )
# set( SDL_TEST OFF CACHE BOOL "" FORCE )

add_subdirectory( SDL )
target_link_libraries( ${CMAKE_PROJECT_NAME} SDL2::SDL2 SDL2::SDL2main SDL2::SDL2-static )

add_subdirectory( cglm )
target_include_directories( ${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/cglm/include )

add_subdirectory( glad )

target_include_directories( ${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/glad/include )
target_include_directories( ${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/stb )
