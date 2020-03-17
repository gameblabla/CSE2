cmake_minimum_required(VERSION 3.12)

option(SUBDIRECTORY "Use clownaudio as a subdirectory, instead of an external package" OFF)

project(clownaudio_test LANGUAGES CXX)

add_executable(test "test.cpp")

if(SUBDIRECTORY)
	add_subdirectory(".." "clownaudio" EXCLUDE_FROM_ALL)
else()
	find_package(clownaudio REQUIRED)
endif()

target_link_libraries(test PRIVATE clownaudio)

set_target_properties(test PROPERTIES
	CXX_STANDARD 98
	CXX_EXTENSIONS OFF
)

if(MSVC)
	target_compile_definitions(test PRIVATE _CRT_SECURE_NO_WARNINGS)	# Shut up those stupid warnings
endif()
