add_executable(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})

find_package(OpenGL REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} ${OPENGL_LIBRARIES})

file(GLOB_RECURSE SOURCES "*.cpp")
set(GLAD_SOURCE ${CMAKE_SOURCE_DIR}/external/glad/src/glad.c) # do it this way atm...
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${SOURCES} ${GLAD_SOURCE})
