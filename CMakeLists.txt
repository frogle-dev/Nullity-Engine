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


option(USE_EDITOR "use the editor" OFF)

add_subdirectory(engine)

if(USE_EDITOR)
    add_subdirectory(editor)
endif()

add_subdirectory(game)
