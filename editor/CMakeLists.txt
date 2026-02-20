file(GLOB editorSource "src/*.cpp")

add_library(NullityEditor STATIC
	${editorSource}
)

add_library(imgui
    # main imgui stuff
    ../lib/imgui/imgui.cpp
    ../lib/imgui/imgui.h
    ../lib/imgui/imgui_draw.cpp
    ../lib/imgui/imgui_tables.cpp
    ../lib/imgui/imgui_widgets.cpp
    ../lib/imgui/imgui_demo.cpp
    # glfw + opengl backends
    ../lib/imgui/backends/imgui_impl_glfw.cpp
    ../lib/imgui/backends/imgui_impl_glfw.h
    ../lib/imgui/backends/imgui_impl_opengl3.cpp
    ../lib/imgui/backends/imgui_impl_opengl3.h
)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
target_include_directories(imgui PUBLIC ../lib/imgui/ ../lib/imgui/backends/)



target_include_directories(NullityEditor
	PRIVATE include
	PUBLIC .
)

target_link_libraries(NullityEditor PUBLIC
	Nullity
	imgui
)
