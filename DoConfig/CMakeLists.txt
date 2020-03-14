cmake_minimum_required(VERSION 3.12)

option(LTO "Enable link-time optimisation" OFF)

option(FORCE_LOCAL_LIBS "Compile the built-in version of FLTK instead of using the system-provided one" OFF)

project(DoConfig LANGUAGES CXX)

add_executable(DoConfig WIN32 "DoConfig.cpp" "icon.rc")

# Make some tweaks if we're using MSVC
if(MSVC)
	# Disable warnings that normally fire up on MSVC when using "unsafe" functions instead of using MSVC's "safe" _s functions
	target_compile_definitions(DoConfig PRIVATE _CRT_SECURE_NO_WARNINGS)

	# Make it so source files are recognized as UTF-8 by MSVC
	target_compile_options(DoConfig PRIVATE "/utf-8")
endif()

if(LTO)
	include(CheckIPOSupported)

	check_ipo_supported(RESULT result)

	if(result)
		set_target_properties(DoConfig PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
	endif()
endif()

# Find FLTK
if(NOT FORCE_LOCAL_LIBS)
	set(FLTK_SKIP_FLUID ON)	# Do not require fltk-fluid (the UI designer)
	find_package(FLTK)
endif()

if(FLTK_FOUND)
	message(STATUS "Using system FLTK")
	target_include_directories(DoConfig PRIVATE ${FLTK_INCLUDE_DIR})
	target_link_libraries(DoConfig PRIVATE ${FLTK_LIBRARIES})
else()
	# Compile it ourselves
	message(STATUS "Using local FLTK")
	# Clear this or it will cause an error during FLTK's configuration.
	# FLTK only appends to it, so the leftover junk gets fed into a bunch
	# of important functions. THAT was no fun to debug.
	set(FLTK_LIBRARIES)
	set(OPTION_BUILD_EXAMPLES OFF)	# Needed to prevent a name collision
	if(FORCE_LOCAL_LIBS)
		set(OPTION_USE_SYSTEM_ZLIB OFF)
		set(OPTION_USE_SYSTEM_LIBJPEG OFF)
		set(OPTION_USE_SYSTEM_LIBPNG OFF)
	endif()
	add_subdirectory("fltk" EXCLUDE_FROM_ALL)
	get_target_property(DIRS fltk INCLUDE_DIRECTORIES)	# FLTK doesn't mark its includes as PUBLIC or INTERFACE, so we have to do this stupidity
	target_include_directories(DoConfig PRIVATE ${DIRS})
	target_link_libraries(DoConfig PRIVATE fltk)
endif()
