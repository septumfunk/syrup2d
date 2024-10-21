cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
project(
    syrup2d
    VERSION 0.1
    LANGUAGES C
)

# Sources
add_executable(${PROJECT_NAME}
    syrup2d.rc

    src/main.c
    src/engine.c

    src/graphics/color.c
    src/graphics/renderer.c
    src/graphics/shader.c

    src/input/keyboard_mouse.c

    src/resources/fs.c
    src/resources/resource_manager.c
    src/resources/sprite.c

    src/scripting/object_manager.c
    src/scripting/scripting_api.c

    src/scripting/modules/modules.c
    src/scripting/modules/graphics.c
    src/scripting/modules/objects.c
    src/scripting/modules/input.c
    src/scripting/modules/resources.c

    src/structures/hashtable.c
    src/structures/result.c

    src/util/crypto.c
    src/util/log.c
    src/util/msgbox.c
    src/util/stringext.c
)

# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/dependencies/cglm/lib/cglm${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CMAKE_SOURCE_DIR}/dependencies/glad/lib/glad${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CMAKE_SOURCE_DIR}/dependencies/glfw3/lib/glfw3${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CMAKE_SOURCE_DIR}/dependencies/stb/lib/stb${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CMAKE_SOURCE_DIR}/dependencies/md5/lib/md5${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CMAKE_SOURCE_DIR}/dependencies/lua5.4/lib/lua54${CMAKE_STATIC_LIBRARY_SUFFIX}
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