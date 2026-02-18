file(GLOB editorSource "src/*.cpp")

add_library(NullityEditor STATIC
	${editorSource}
)

target_include_directories(NullityEditor PUBLIC
	include
	.
)

target_link_libraries(NullityEditor PUBLIC
	Nullity
	imgui
)
