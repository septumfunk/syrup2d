cmake_minimum_required(VERSION 3.29)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(
    rendertest
    VERSION 0.1
    LANGUAGES C
)

# Sources
add_executable(${PROJECT_NAME}
    src/main.c

    src/data/fs.c
    src/data/stringext.c
    src/data/vector.c

    src/game/game.c

    src/graphics/opengl.c
    src/graphics/color.c
    src/graphics/shader.c

    src/systems/window.c
    src/systems/controller.c

    src/win32/msgbox.c
)

# Libraries
find_package(cglm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE
    cglm::cglm
    glad::glad
    glfw
)

# Compile Options
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)