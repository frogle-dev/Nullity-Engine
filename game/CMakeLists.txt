file(GLOB gameSource "src/*.cpp")

add_executable(Game
	${gameSource}
)

target_include_directories(Game PRIVATE
	include
)

target_link_libraries(Game PRIVATE Nullity)

if(USE_EDITOR)
	target_link_libraries(Game PRIVATE NullityEditor)
	target_compile_definitions(Game PRIVATE USE_EDITOR)
endif()
