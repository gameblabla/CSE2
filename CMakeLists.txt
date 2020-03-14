cmake_minimum_required(VERSION 3.12)

set(ASSETS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")

option(JAPANESE "Enable the Japanese-language build" OFF)
option(FIX_BUGS "Fix various bugs in the game" ON)
option(DEBUG_SAVE "Re-enable the ability to drag-and-drop save files onto the window" OFF)

option(LTO "Enable link-time optimisation" OFF)
option(NATIVE_OPTIMIZATIONS "Enable processor-specific optimisations (executable might not work on other architectures) (GCC-compatible compilers only)" OFF)

option(WARNINGS "Enable common compiler warnings (for GCC-compatible compilers and MSVC only)" OFF)
option(WARNINGS_ALL "Enable ALL compiler warnings (for Clang and MSVC only)" OFF)
option(WARNINGS_FATAL "Stop compilation on any compiler warning (for GCC-compatible compilers and MSVC only)" OFF)

project(CSE2 LANGUAGES C CXX)

message(STATUS "Compiler ID : ${CMAKE_CXX_COMPILER_ID}")

# Has to be placed after "project()"
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# Using Clang (this is a match so that we also get "AppleClang" which is the Apple-provided Clang
	set(COMPILER_IS_CLANG true)
	message(STATUS "Compiling with Clang")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	# Using GCC
	set(COMPILER_IS_GCC true)
	message(STATUS "Compiling with GCC")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	# Using Intel C++
	set(COMPILER_IS_ICC true)
	message(STATUS "Compiling with ICC")
endif()

if(COMPILER_IS_CLANG OR COMPILER_IS_GCC OR COMPILER_IS_ICC)
	set(COMPILER_IS_GCC_COMPATIBLE true)
	message(STATUS "Compiling with a GCC-compatible compiler")
endif()

if(MSVC)
	# Statically-link the CRT (vcpkg static libs do this)
	foreach(flag_var CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		if(${flag_var} MATCHES "/MD")
			string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
		endif()
	endforeach()
endif()

##
# CSE2
##

add_executable(CSE2
	"src/ArmsItem.cpp"
	"src/ArmsItem.h"
	"src/Back.cpp"
	"src/Back.h"
	"src/Boss.cpp"
	"src/Boss.h"
	"src/BossAlmo1.cpp"
	"src/BossAlmo1.h"
	"src/BossAlmo2.cpp"
	"src/BossAlmo2.h"
	"src/BossBallos.cpp"
	"src/BossBallos.h"
	"src/BossFrog.cpp"
	"src/BossFrog.h"
	"src/BossIronH.cpp"
	"src/BossIronH.h"
	"src/BossLife.cpp"
	"src/BossLife.h"
	"src/BossOhm.cpp"
	"src/BossOhm.h"
	"src/BossPress.cpp"
	"src/BossPress.h"
	"src/BossTwinD.cpp"
	"src/BossTwinD.h"
	"src/BossX.cpp"
	"src/BossX.h"
	"src/BulHit.cpp"
	"src/BulHit.h"
	"src/Bullet.cpp"
	"src/Bullet.h"
	"src/Caret.cpp"
	"src/Caret.h"
	"src/CommonDefines.h"
	"src/Config.cpp"
	"src/Config.h"
	"src/Dialog.cpp"
	"src/Dialog.h"
	"src/Draw.cpp"
	"src/Draw.h"
	"src/Ending.cpp"
	"src/Ending.h"
	"src/Escape.cpp"
	"src/Escape.h"
	"src/Fade.cpp"
	"src/Fade.h"
	"src/Flags.cpp"
	"src/Flags.h"
	"src/Flash.cpp"
	"src/Flash.h"
	"src/Frame.cpp"
	"src/Frame.h"
	"src/Game.cpp"
	"src/Game.h"
	"src/Generic.cpp"
	"src/Generic.h"
	"src/GenericLoad.cpp"
	"src/GenericLoad.h"
	"src/Input.cpp"
	"src/Input.h"
	"src/KeyControl.cpp"
	"src/KeyControl.h"
	"src/Main.cpp"
	"src/Main.h"
	"src/Map.cpp"
	"src/Map.h"
	"src/MapName.cpp"
	"src/MapName.h"
	"src/MiniMap.cpp"
	"src/MiniMap.h"
	"src/MyChar.cpp"
	"src/MyChar.h"
	"src/MycHit.cpp"
	"src/MycHit.h"
	"src/MycParam.cpp"
	"src/MycParam.h"
	"src/NpcAct.h"
	"src/NpcAct000.cpp"
	"src/NpcAct020.cpp"
	"src/NpcAct040.cpp"
	"src/NpcAct060.cpp"
	"src/NpcAct080.cpp"
	"src/NpcAct100.cpp"
	"src/NpcAct120.cpp"
	"src/NpcAct140.cpp"
	"src/NpcAct160.cpp"
	"src/NpcAct180.cpp"
	"src/NpcAct200.cpp"
	"src/NpcAct220.cpp"
	"src/NpcAct240.cpp"
	"src/NpcAct260.cpp"
	"src/NpcAct280.cpp"
	"src/NpcAct300.cpp"
	"src/NpcAct320.cpp"
	"src/NpcAct340.cpp"
	"src/NpChar.cpp"
	"src/NpChar.h"
	"src/NpcHit.cpp"
	"src/NpcHit.h"
	"src/NpcTbl.cpp"
	"src/NpcTbl.h"
	"src/Organya.cpp"
	"src/Organya.h"
	"src/PixTone.cpp"
	"src/PixTone.h"
	"src/Profile.cpp"
	"src/Profile.h"
	"src/SelStage.cpp"
	"src/SelStage.h"
	"src/Shoot.cpp"
	"src/Shoot.h"
	"src/Sound.cpp"
	"src/Sound.h"
	"src/Stage.cpp"
	"src/Stage.h"
	"src/Star.cpp"
	"src/Star.h"
	"src/TextScr.cpp"
	"src/TextScr.h"
	"src/Triangle.cpp"
	"src/Triangle.h"
	"src/ValueView.cpp"
	"src/ValueView.h"
	"src/WindowsWrapper.h"
)

# Handle options
if(JAPANESE)
	set(BUILD_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game_japanese")
	target_compile_definitions(CSE2 PRIVATE JAPANESE)
else()
	set(BUILD_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game_english")
endif()

if(FIX_BUGS)
	target_compile_definitions(CSE2 PRIVATE FIX_BUGS)
endif()

if(DEBUG_SAVE)
	target_compile_definitions(CSE2 PRIVATE DEBUG_SAVE)
endif()

if(WARNINGS)
	# HACK : Replace this with CMake provided stuff when possible (when CMake makes avoiding this possible (it isn't currently))

	if(MSVC)
		# Force to always compile with /W4 on MSVC

		# Can't do this with target_compile_options
		# if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		# 	string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
		# else()
		# 	target_compile_options(CSE2 PRIVATE /W4)
		# endif()

		target_compile_options(CSE2 PRIVATE /W4)
	elseif(COMPILER_IS_GCC_COMPATIBLE)
		target_compile_options(CSE2 PRIVATE -Wall -Wextra -pedantic)
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

		target_compile_options(CSE2 PRIVATE /Wall)
	elseif(COMPILER_IS_CLANG)
		target_compile_options(CSE2 PRIVATE -Weverything)
	else()
		message(WARNING "Could not activate all warnings ! (Unsupported compiler)")
	endif()
endif()

if(WARNINGS_FATAL)
	# HACK : Replace this with CMake provided stuff when possible (when CMake makes avoiding this possible (it isn't currently))

	if(MSVC)
		target_compile_options(CSE2 PRIVATE /WX)
	elseif(COMPILER_IS_GCC_COMPATIBLE)
		target_compile_options(CSE2 PRIVATE -Werror)
	else()
		message(WARNING "Could not activate fatal warnings ! (Unsupported compiler)")
	endif()
endif()

# Make some tweaks if we're targetting Windows
#if(WIN32)
	target_sources(CSE2 PRIVATE "${ASSETS_DIRECTORY}/resources/CSE2.rc")
	set_target_properties(CSE2 PROPERTIES WIN32_EXECUTABLE YES)	# Disable the console window
#endif()

# Make some tweaks if we're using MSVC
if(MSVC)
	# Disable warnings that normally fire up on MSVC when using "unsafe" functions instead of using MSVC's "safe" _s functions
	target_compile_definitions(CSE2 PRIVATE _CRT_SECURE_NO_WARNINGS)

	# Make it so source files are recognized as UTF-8 by MSVC
	target_compile_options(CSE2 PRIVATE "/utf-8")
endif()

# Force strict C90
set_target_properties(CSE2 PROPERTIES
	C_STANDARD 90
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF
)

# Force strict C++98
set_target_properties(CSE2 PROPERTIES
	CXX_STANDARD 98
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

# Name debug builds "CSE2_debug", to distinguish them
set_target_properties(CSE2 PROPERTIES DEBUG_OUTPUT_NAME "CSE2_debug")

# Send executable to the build_en/build_jp directory
set_target_properties(CSE2 PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIRECTORY}
)

# Enable link-time optimisation if available
if(LTO)
	if((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
		include(CheckIPOSupported)
		check_ipo_supported(RESULT result)
		if(result)
			set_target_properties(CSE2 PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
		endif()
	endif()
endif()

# Enable -march=native if available
if(NATIVE_OPTIMIZATIONS)
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)	# GCC flag
	if(COMPILER_SUPPORTS_MARCH_NATIVE)
		target_compile_options(CSE2 PRIVATE -march=native)
	else()
		CHECK_CXX_COMPILER_FLAG("-xHost" COMPILER_SUPPORTS_XHOST)	# ICC (Linux) flag
		CHECK_CXX_COMPILER_FLAG("/QxHost" COMPILER_SUPPORTS_QXHOST)	# ICC (Windows) flag
		if(COMPILER_SUPPORTS_XHOST)
			target_compile_options(CSE2 PRIVATE -xHost)
		elseif(COMPILER_SUPPORTS_QXHOST)
			target_compile_options(CSE2 PRIVATE /QxHost)
		else()
			message(WARNING "Couldn't activate native optimizations ! (Unsupported compiler)")
		endif()
	endif()
endif()

# Link libraries
target_link_libraries(CSE2 PRIVATE ddraw.lib dsound.lib Version.lib ShLwApi.Lib Imm32.lib WinMM.lib dxguid.lib)

# Newer MSVC is missing `dinput.lib`
if(MSVC AND MSVC_VERSION GREATER_EQUAL 1500)
	target_link_libraries(CSE2 PRIVATE dinput8.lib)
else()
	target_link_libraries(CSE2 PRIVATE dinput.lib)
endif()
