cmake_minimum_required(VERSION 3.12)

option(SUBDIRECTORY "Use clownaudio as a subdirectory, instead of an external package" OFF)

project(clownaudio_test LANGUAGES C CXX)

add_executable(test
	"test.cpp"
	"glad/include/glad/glad.h"
	"glad/include/KHR/khrplatform.h"
	"glad/src/glad.c"
	"imgui/imconfig.h"
	"imgui/imgui.cpp"
	"imgui/imgui.h"
	"imgui/imgui_demo.cpp"
	"imgui/imgui_draw.cpp"
	"imgui/imgui_impl_opengl3.cpp"
	"imgui/imgui_impl_opengl3.h"
	"imgui/imgui_impl_sdl.cpp"
	"imgui/imgui_impl_sdl.h"
	"imgui/imgui_internal.h"
	"imgui/imgui_widgets.cpp"
	"imgui/imstb_rectpack.h"
	"imgui/imstb_textedit.h"
	"imgui/imstb_truetype.h"
)

if(SUBDIRECTORY)
	add_subdirectory(".." "clownaudio" EXCLUDE_FROM_ALL)
else()
	find_package(clownaudio REQUIRED)
endif()

target_link_libraries(test PRIVATE clownaudio)

set_target_properties(test PROPERTIES
	C_STANDARD 90
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF
	CXX_STANDARD 98
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

target_include_directories(test PRIVATE "glad/include")
target_compile_definitions(test PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

if(MSVC)
	target_compile_definitions(test PRIVATE _CRT_SECURE_NO_WARNINGS)	# Shut up those stupid warnings
endif()

find_package(SDL2 REQUIRED)

if(TARGET SDL2::SDL2)
	# CMake-generated config (Arch, vcpkg, Raspbian)
	message(STATUS "Using system SDL2 (CMake, dynamic)")
	target_link_libraries(test PRIVATE SDL2::SDL2 SDL2::SDL2main)
elseif(TARGET SDL2::SDL2-static)
	# CMake-generated config (Arch, vcpkg, Raspbian)
	message(STATUS "Using system SDL2 (CMake, static)")
	target_link_libraries(test PRIVATE SDL2::SDL2-static SDL2::SDL2main)
elseif(SDL2_FOUND)
	# Autotools-generated config (MSYS2)
	message(STATUS "Using system SDL2 (Autotools)")
	target_include_directories(CSE2 PRIVATE ${SDL2_INCLUDE_DIRS})
	target_link_libraries(test PRIVATE ${SDL2_LIBRARIES})
endif()
