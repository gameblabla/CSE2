cmake_minimum_required(VERSION 3.8)

option(DOCONFIG_LEGACY_OPENGL "Use OpenGL 2.1 instead of 3.2" OFF)

option(PKG_CONFIG_STATIC_LIBS "On platforms with pkg-config, static-link the dependencies (good for Windows builds, so you don't need to bundle DLL files)" OFF)
option(MSVC_LINK_STATIC_RUNTIME "Link the static MSVC runtime library (Visual Studio only)" OFF)

option(FORCE_LOCAL_LIBS "Compile the built-in version of GLFW3 instead of using the system-provided one" OFF)

project(DoConfig LANGUAGES C CXX)

add_executable(DoConfig WIN32
	"icon.rc"
	"DoConfig.cpp"
	"imgui/imconfig.h"
	"imgui/imgui.cpp"
	"imgui/imgui.h"
	"imgui/imgui_demo.cpp"
	"imgui/imgui_draw.cpp"
	"imgui/imgui_impl_glfw.cpp"
	"imgui/imgui_impl_glfw.h"
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

target_include_directories(DoConfig PRIVATE "../external/glad/include")
target_compile_definitions(DoConfig PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

if(DOCONFIG_LEGACY_OPENGL)
	target_compile_definitions(DoConfig PRIVATE LEGACY_OPENGL)
	target_sources(DoConfig PRIVATE
		"imgui/imgui_impl_opengl2.cpp"
		"imgui/imgui_impl_opengl2.h"
	)
else()
	target_sources(DoConfig PRIVATE
		"imgui/imgui_impl_opengl3.cpp"
		"imgui/imgui_impl_opengl3.h"
	)
endif()

if(PKG_CONFIG_STATIC_LIBS)
	target_link_options(DoConfig PRIVATE "-static")
endif()

if(MSVC)
	# Disable warnings that normally fire up on MSVC when using "unsafe" functions instead of using MSVC's "safe" _s functions
	target_compile_definitions(DoConfig PRIVATE _CRT_SECURE_NO_WARNINGS)

	# Use `main` instead of `WinMain`
	set_target_properties(DoConfig PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")

	# This is messy as hell, and has been replaced by CMAKE_MSVC_RUNTIME_LIBRARY,
	# but that's a very recent CMake addition, so we're still doing it this way for now
	if(MSVC_LINK_STATIC_RUNTIME)
		# Statically-link the CRT (vcpkg static libs do this)
		foreach(flag_var CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
			if(${flag_var} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
			endif()
		endforeach()
	endif()
endif()


################
# Dependencies #
################

# glad

add_subdirectory("../external/glad" "glad" EXCLUDE_FROM_ALL)
target_link_libraries(DoConfig PRIVATE glad)

if (CMAKE_VERSION GREATER_EQUAL 3.11)
	cmake_policy(SET CMP0072 NEW)
endif()

find_package(OpenGL REQUIRED)
target_link_libraries(DoConfig PRIVATE OpenGL::GL)

# GLFW3

if(NOT FORCE_LOCAL_LIBS)
	find_package(PkgConfig QUIET)
endif()

if(NOT FORCE_LOCAL_LIBS)
	find_package(glfw3)

	if (PKG_CONFIG_FOUND)
		pkg_check_modules(glfw3 QUIET IMPORTED_TARGET glfw3)
	endif()
endif()

if(TARGET PkgConfig::glfw3)
	# pkg-config
	if (PKG_CONFIG_STATIC_LIBS)
		message(STATUS "Using system GLFW3 (pkg-config, static)")
		target_compile_options(DoConfig PRIVATE ${glfw3_STATIC_CFLAGS})
		target_link_libraries(DoConfig PRIVATE ${glfw3_STATIC_LDFLAGS})
	else()
		message(STATUS "Using system GLFW3 (pkg-config, dynamic)")
		target_compile_options(DoConfig PRIVATE ${glfw3_CFLAGS})
		target_link_libraries(DoConfig PRIVATE ${glfw3_LDFLAGS})
	endif()
elseif(TARGET glfw)
	# CMake
	message(STATUS "Using system GLFW3 (CMake)")
	target_link_libraries(DoConfig PRIVATE glfw)
else()
	# Compile it ourselves
	message(STATUS "Using local GLFW3")
	set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
	set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "" FORCE)
	set(GLFW_INSTALL OFF CACHE INTERNAL "" FORCE)
	add_subdirectory("../external/glfw" "glfw" EXCLUDE_FROM_ALL)
	target_link_libraries(DoConfig PRIVATE glfw)
endif()
