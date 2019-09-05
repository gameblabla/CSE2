cmake_minimum_required(VERSION 3.7.2)

if((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
	cmake_policy(SET CMP0069 NEW)
endif()

option(FORCE_LOCAL_LIBS "Compile the built-in version of FLTK instead of using the system-provided one" OFF)

project(DoConfig LANGUAGES CXX)

add_executable(DoConfig "DoConfig.cpp" "icon.rc")

# Windows tweak
if(WIN32)
	set_target_properties(DoConfig PROPERTIES WIN32_EXECUTABLE YES)	# Disable the console window
endif()

# MSVC tweak
if(MSVC)
	target_compile_definitions(DoConfig PRIVATE _CRT_SECURE_NO_WARNINGS)	# Shut up those stupid warnings
endif()

# Find FLTK
if(NOT FORCE_LOCAL_LIBS)
	find_package(FLTK)
endif()

if(FLTK_FOUND)
	message(STATUS "Using system FLTK")
	target_include_directories(DoConfig PRIVATE ${FLTK_INCLUDE_DIR})
	target_link_libraries(DoConfig ${FLTK_LIBRARIES})
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
	target_link_libraries(DoConfig fltk)
endif()

# Enable link-time optimisation if available
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	if((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
		include(CheckIPOSupported)
		check_ipo_supported(RESULT result)
		if(result)
			set_target_properties(DoConfig PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
		endif()
	endif()
endif()
