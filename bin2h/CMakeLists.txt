cmake_minimum_required(VERSION 3.8)

option(LTO "Enable link-time optimisation" OFF)

project(bin2h LANGUAGES C)

add_executable(bin2h "bin2h.c")

set_target_properties(bin2h PROPERTIES
	C_STANDARD 90
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF
)

# Make some tweaks if we're using MSVC
if(MSVC)
	# Disable warnings that normally fire up on MSVC when using "unsafe" functions instead of using MSVC's "safe" _s functions
	target_compile_definitions(bin2h PRIVATE _CRT_SECURE_NO_WARNINGS)

	# Make it so source files are recognized as UTF-8 by MSVC
	target_compile_options(bin2h PRIVATE "/utf-8")
endif()

if(LTO)
	include(CheckIPOSupported)

	check_ipo_supported(RESULT result)

	if(result)
		set_target_properties(bin2h PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
	endif()
endif()

install(TARGETS bin2h RUNTIME DESTINATION bin)
