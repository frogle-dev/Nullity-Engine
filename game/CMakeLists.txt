file(GLOB gameSource "src/*.cpp")

add_executable(Game
	${gameSource}
)

target_include_directories(Game PRIVATE
	include
)

target_link_libraries(Game PRIVATE Nullity)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_link_libraries(Game PRIVATE NullityEditor)
	add_compile_definitions(DEBUG)
endif()
