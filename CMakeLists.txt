cmake_minimum_required(VERSION 3.10.0)
project(opengl_game VERSION 0.1.0 LANGUAGES C CXX)
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

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
target_include_directories(imgui PUBLIC lib/imgui/ lib/imgui/backends/)


add_subdirectory(engine)
add_subdirectory(editor)
add_subdirectory(game)
