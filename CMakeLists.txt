cmake_minimum_required(VERSION 3.29)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(
    rendertest
    VERSION 0.1
    LANGUAGES C
)

# Sources
file(GLOB_RECURSE SOURCES src/*.c)
add_executable(${PROJECT_NAME} ${SOURCES})

# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/dependencies/cglm/lib/cglm.lib
    ${CMAKE_SOURCE_DIR}/dependencies/glad/lib/glad.lib
    ${CMAKE_SOURCE_DIR}/dependencies/glfw3/lib/glfw3.lib
)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/dependencies/cglm/include
    ${CMAKE_SOURCE_DIR}/dependencies/glad/include
    ${CMAKE_SOURCE_DIR}/dependencies/glfw3/include
    ${CMAKE_SOURCE_DIR}/dependencies/stb/include
)

# Compile Options
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)