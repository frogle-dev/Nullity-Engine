file(GLOB engineSource "src/*.cpp")

add_library(Nullity STATIC
	${engineSource}
	${CMAKE_SOURCE_DIR}/lib/glad.c
)

target_include_directories(Nullity PUBLIC 
	include 
	${CMAKE_SOURCE_DIR}/lib
	.
)

target_link_libraries(Nullity PUBLIC
	glfw
	OpenGL::GL
	assimp
	EnTT::EnTT
	X11
)


