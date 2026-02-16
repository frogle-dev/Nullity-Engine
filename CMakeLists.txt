cmake_minimum_required(VERSION 3.10.0)
project(nullity VERSION 0.1.0 LANGUAGES C CXX)
cmake_policy(SET CMP0072 NEW) #make sure "new" opengl is used

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.16.0  # Use latest stable version
)
FetchContent_MakeAvailable(EnTT)


find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)

add_library(imgui
    # main imgui stuff
    lib/imgui/imgui.cpp
    lib/imgui/imgui.h
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imgui_demo.cpp
    # glfw + opengl backends
    lib/imgui/backends/imgui_impl_glfw.cpp
    lib/imgui/backends/imgui_impl_glfw.h
    lib/imgui/backends/imgui_impl_opengl3.cpp
    lib/imgui/backends/imgui_impl_opengl3.h
)

target_link_libraries(imgui
    PUBLIC
    glfw
    OpenGL::GL
)
target_include_directories(imgui PUBLIC lib/imgui/ lib/imgui/backends/)


add_executable(nullity 
src/main.cpp
src/camera.cpp
src/debugging.cpp
src/framebuffer.cpp
src/engine_gui.cpp
src/init.cpp
src/keymap.cpp
src/models.cpp
src/player.cpp
src/render.cpp
src/systems.cpp
src/textures.cpp
src/utility.cpp
src/shader.cpp
src/engine.cpp
lib/glad.c
)

target_include_directories(nullity
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(nullity 
glfw
OpenGL::GL
assimp
EnTT::EnTT
imgui
X11
)
