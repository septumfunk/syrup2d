cmake_minimum_required(VERSION 3.29)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(MSVC_RUNTIME_LIBRARY MultiThreaded)
project(
    syrup-engine
    VERSION 0.1
    LANGUAGES C
)

# Sources
add_executable(${PROJECT_NAME}
    syrup-engine.rc

    src/main.c

    src/data/fs.c
    src/data/stringext.c
    src/data/vector.c
    src/data/hashtable.c
    src/data/result.c
    src/data/crypto.c

    src/graphics/shader.c
    src/graphics/color.c
    src/graphics/sprite.c

    src/engine/engine.c
    src/engine/controller.c
    src/engine/window.c
    src/engine/renderer.c
    src/engine/sprite_manager.c
    src/engine/object_controller.c

    src/util/log.c

    src/win32/msgbox.c
    src/win32/files.c
)

# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/dependencies/cglm/lib/cglm.lib
    ${CMAKE_SOURCE_DIR}/dependencies/glad/lib/glad.lib
    ${CMAKE_SOURCE_DIR}/dependencies/glfw3/lib/glfw3.lib
    ${CMAKE_SOURCE_DIR}/dependencies/stb/lib/stb.lib
    ${CMAKE_SOURCE_DIR}/dependencies/md5/lib/md5.lib
    ${CMAKE_SOURCE_DIR}/dependencies/lua5.4/lib/lua54.lib
)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/dependencies/cglm/include
    ${CMAKE_SOURCE_DIR}/dependencies/glad/include
    ${CMAKE_SOURCE_DIR}/dependencies/glfw3/include
    ${CMAKE_SOURCE_DIR}/dependencies/stb/include
    ${CMAKE_SOURCE_DIR}/dependencies/md5/include
    ${CMAKE_SOURCE_DIR}/dependencies/lua5.4/include
)

# Compile Options
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)