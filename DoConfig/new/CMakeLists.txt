cmake_minimum_required(VERSION 3.12)

project(DoConfig LANGUAGES C CXX)

add_executable(DoConfig WIN32
	"icon.rc"
	"../../external/glad/include/glad/glad.h"
	"../../external/glad/include/KHR/khrplatform.h"
	"../../external/glad/src/glad.c"
	"DoConfig.cpp"
	"imgui/imconfig.h"
	"imgui/imgui.cpp"
	"imgui/imgui.h"
	"imgui/imgui_demo.cpp"
	"imgui/imgui_draw.cpp"
	"imgui/imgui_impl_glfw.cpp"
	"imgui/imgui_impl_glfw.h"
	"imgui/imgui_impl_opengl3.cpp"
	"imgui/imgui_impl_opengl3.h"
	"imgui/imgui_internal.h"
	"imgui/imgui_widgets.cpp"
	"imgui/imstb_rectpack.h"
	"imgui/imstb_textedit.h"
	"imgui/imstb_truetype.h"
)

set_target_properties(DoConfig PROPERTIES
	C_STANDARD 90
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF
	CXX_STANDARD 98
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

target_link_libraries(DoConfig PRIVATE ${CMAKE_DL_LIBS})

target_include_directories(DoConfig PRIVATE "../../external/glad/include")
target_compile_definitions(DoConfig PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

if(MSVC)
	# Disable warnings that normally fire up on MSVC when using "unsafe" functions instead of using MSVC's "safe" _s functions
	target_compile_definitions(CSE2 PRIVATE _CRT_SECURE_NO_WARNINGS)

	# Use `main` instead of `WinMain`
	set_target_properties(CSE2 PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

find_package(glfw3 REQUIRED)
target_link_libraries(DoConfig PRIVATE glfw)
