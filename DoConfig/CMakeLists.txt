cmake_minimum_required(VERSION 3.7.2)

if((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
	cmake_policy(SET CMP0069 NEW)
endif()

if((${CMAKE_VERSION} VERSION_EQUAL 3.11) OR (${CMAKE_VERSION} VERSION_GREATER 3.11))
	cmake_policy(SET CMP0072 NEW)
endif()

option(WARNINGS "Enable common compiler warnings (for gcc-compatible compilers and MSVC only)" OFF)
option(WARNINGS_ALL "Enable ALL compiler warnings (for clang and MSVC only)" OFF)
option(WARNINGS_FATAL "Stop compilation on any compiler warning (for gcc-compatible compilers and MSVC only)" OFF)
option(FORCE_LOCAL_LIBS "Compile the built-in version of FLTK instead of using the system-provided one" OFF)

project(DoConfig LANGUAGES CXX)

message(STATUS "Compiler ID : ${CMAKE_CXX_COMPILER_ID}")

# Has to be placed after "project()"
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# Using Clang (this is a match so that we also get "AppleClang" which is the Apple-provided Clang
	set(COMPILER_IS_CLANG true)
	message(STATUS "Compiling with clang")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	# Using GCC
	set(COMPILER_IS_GCC true)
	message(STATUS "Compiling with gcc")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	# Using Intel C++
	set(COMPILER_IS_ICC true)
	message(STATUS "Compiling with ICC")
endif()

if (COMPILER_IS_CLANG OR COMPILER_IS_GCC OR COMPILER_IS_ICC)
	set(COMPILER_IS_GCC_COMPATIBLE true)
	message(STATUS "Compiling with a GCC-compatible compiler")
endif()

add_executable(DoConfig "DoConfig.cpp" "icon.rc")

if (WARNINGS)
	# HACK : Replace this with CMake provided stuff when possible (when CMake makes avoiding this possible (it isn't currently))

	if (MSVC)
		# Force to always compile with /W4 on MSVC

		# Can't do this with target_compile_options
		# if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		# 	string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
		# else()
		# 	target_compile_options(CSE2 PRIVATE /W4)
		# endif()

		target_compile_options(DoConfig PRIVATE /W4)
	elseif(COMPILER_IS_GCC_COMPATIBLE)
		target_compile_options(DoConfig PRIVATE -Wall -Wextra -pedantic)
	else()
		message(WARNING "Could not activate warnings ! (Unsupported compiler)")
	endif()
endif()

if (WARNINGS_ALL)
	# HACK : Replace this with CMake provided stuff when possible (when CMake makes avoiding this possible (it isn't currently))

	if (MSVC)
		# Force to always compile with /Wall on MSVC

		# Can't do this with target_compile_options
		# if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		# 	string(REGEX REPLACE "/W[0-4]" "/Wall" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
		# else()
		# 	target_compile_options(CSE2 PRIVATE /Wall)
		# endif()

		target_compile_options(DoConfig PRIVATE /Wall)
	elseif(COMPILER_IS_CLANG)
		target_compile_options(DoConfig PRIVATE -Weverything)
	else()
		message(WARNING "Could not activate all warnings ! (Unsupported compiler)")
	endif()
endif()

if (WARNINGS_FATAL)
	# HACK : Replace this with CMake provided stuff when possible (when CMake makes avoiding this possible (it isn't currently))

	if (MSVC)
		target_compile_options(DoConfig PRIVATE /WX)
	elseif(COMPILER_IS_GCC_COMPATIBLE)
		target_compile_options(DoConfig PRIVATE -Werror)
	else()
		message(WARNING "Could not activate fatal warnings ! (Unsupported compiler)")
	endif()
endif()

# Windows tweak
if(WIN32)
	set_target_properties(DoConfig PROPERTIES WIN32_EXECUTABLE YES)	# Disable the console window
endif()

# MSVC tweak
if(MSVC)
	target_compile_definitions(DoConfig PRIVATE _CRT_SECURE_NO_WARNINGS)	# Disable warnings that normally fire up on MSVC when using "unsafe" functions instead of using MSVC's "safe" _s functions
endif()

# Make it so source files are recognized as UTF-8 by MSVC
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Find FLTK
if(NOT FORCE_LOCAL_LIBS)
	set(FLTK_SKIP_FLUID ON)	# Do not require fltk-fluid (the UI designer)
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
