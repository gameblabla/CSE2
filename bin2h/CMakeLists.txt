cmake_minimum_required(VERSION 3.7.2)

if((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
	cmake_policy(SET CMP0069 NEW)
endif()

option(LTO "Enable link-time optimisation" OFF)
option(NATIVE_OPTIMIZATIONS "Enable processor-specific optimisations (executable might not work on other architectures) (GCC-compatible compilers only)" OFF)

option(WARNINGS "Enable common compiler warnings (for GCC-compatible compilers and MSVC only)" OFF)
option(WARNINGS_ALL "Enable ALL compiler warnings (for Clang and MSVC only)" OFF)
option(WARNINGS_FATAL "Stop compilation on any compiler warning (for GCC-compatible compilers and MSVC only)" OFF)

project(bin2h LANGUAGES C)

add_executable(bin2h "bin2h.c")

set_target_properties(bin2h PROPERTIES
	C_STANDARD 90
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF
)

message(STATUS "Compiler ID : ${CMAKE_C_COMPILER_ID}")

# Has to be placed after "project()"
if (CMAKE_C_COMPILER_ID MATCHES "Clang")
	# Using Clang (this is a match so that we also get "AppleClang" which is the Apple-provided Clang
	set(COMPILER_IS_CLANG true)
	message(STATUS "Compiling with Clang")
endif()

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
	# Using GCC
	set(COMPILER_IS_GCC true)
	message(STATUS "Compiling with GCC")
endif()

if (CMAKE_C_COMPILER_ID STREQUAL "Intel")
	# Using Intel C++
	set(COMPILER_IS_ICC true)
	message(STATUS "Compiling with ICC")
endif()

if (COMPILER_IS_CLANG OR COMPILER_IS_GCC OR COMPILER_IS_ICC)
	set(COMPILER_IS_GCC_COMPATIBLE true)
	message(STATUS "Compiling with a GCC-compatible compiler")
endif()

if (WARNINGS)
	# HACK : Replace this with CMake provided stuff when possible (when CMake makes avoiding this possible (it isn't currently))

	if (MSVC)
		# Force to always compile with /W4 on MSVC

		# Can't do this with target_compile_options
		# if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		# 	string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
		# else()
		# 	target_compile_options(bin2h PRIVATE /W4)
		# endif()

		target_compile_options(bin2h PRIVATE /W4)
	elseif(COMPILER_IS_GCC_COMPATIBLE)
		target_compile_options(bin2h PRIVATE -Wall -Wextra -pedantic)
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
		# 	target_compile_options(bin2h PRIVATE /Wall)
		# endif()

		target_compile_options(bin2h PRIVATE /Wall)
	elseif(COMPILER_IS_CLANG)
		target_compile_options(bin2h PRIVATE -Weverything)
	else()
		message(WARNING "Could not activate all warnings ! (Unsupported compiler)")
	endif()
endif()

if (WARNINGS_FATAL)
	# HACK : Replace this with CMake provided stuff when possible (when CMake makes avoiding this possible (it isn't currently))

	if (MSVC)
		target_compile_options(bin2h PRIVATE /WX)
	elseif(COMPILER_IS_GCC_COMPATIBLE)
		target_compile_options(bin2h PRIVATE -Werror)
	else()
		message(WARNING "Could not activate fatal warnings ! (Unsupported compiler)")
	endif()
endif()

# MSVC tweak
if(MSVC)
	target_compile_definitions(bin2h PRIVATE _CRT_SECURE_NO_WARNINGS)	# Disable warnings that normally fire up on MSVC when using "unsafe" functions instead of using MSVC's "safe" _s functions
endif()

# Make it so source files are recognized as UTF-8 by MSVC
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")

# Enable link-time optimisation if available
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	if((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
		include(CheckIPOSupported)
		check_ipo_supported(RESULT result)
		if(result)
			set_target_properties(bin2h PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
		endif()
	endif()
endif()

# Enable link-time optimisation if available
if(LTO)
	if((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
		include(CheckIPOSupported)
		check_ipo_supported(RESULT result)
		if(result)
			set_target_properties(bin2h PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
		endif()
	endif()
endif()

# Enable -march=native if available
if(NATIVE_OPTIMIZATIONS)
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)	# GCC flag
	if(COMPILER_SUPPORTS_MARCH_NATIVE)
		target_compile_options(bin2h PRIVATE -march=native)
	else()
		CHECK_CXX_COMPILER_FLAG("-xHost" COMPILER_SUPPORTS_XHOST)	# ICC (Linux) flag
		CHECK_CXX_COMPILER_FLAG("/QxHost" COMPILER_SUPPORTS_QXHOST)	# ICC (Windows) flag
		if(COMPILER_SUPPORTS_XHOST)
			target_compile_options(bin2h PRIVATE -xHost)
		elseif(COMPILER_SUPPORTS_QXHOST)
			target_compile_options(bin2h PRIVATE /QxHost)
		else()
			message(WARNING "Couldn't activate native optimizations ! (Unsupported compiler)")
		endif()
	endif()
endif()

install(TARGETS bin2h RUNTIME DESTINATION bin)
