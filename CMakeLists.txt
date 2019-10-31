cmake_minimum_required(VERSION 3.7.2)

if((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
	cmake_policy(SET CMP0069 NEW)
endif()

if((${CMAKE_VERSION} VERSION_EQUAL 3.11) OR (${CMAKE_VERSION} VERSION_GREATER 3.11))
	cmake_policy(SET CMP0072 NEW)
endif()

set(BUILD_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game")
set(ASSETS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")

option(JAPANESE "Enable the Japanese-language build" OFF)
option(FIX_BUGS "Fix various bugs in the game" ON)
option(DEBUG_SAVE "Re-enable the ability to drag-and-drop save files onto the window" OFF)
set(RENDERER "SDLTexture" CACHE STRING "Which renderer the game should use: 'OpenGL3' for an OpenGL 3.2 renderer, 'SDLTexture' for SDL2's hardware-accelerated Texture API, or 'Software' for a handwritten software renderer")
option(AUDIO_OGG "Enable support for Ogg Vorbis music/SFX" OFF)
option(AUDIO_FLAC "Enable support for FLAC music/SFX" OFF)
option(AUDIO_TRACKER "Enable support for .it, .xm, .mod, .s3m music/SFX" OFF)
option(AUDIO_PXTONE "Enable support for PxTone music/SFX (not to be confused with PixTone)" OFF)
option(SMOOTH_SPRITE_MOVEMENT "Removes the grid that all sprites are locked to, allowing them to move smoothly at higher resolutions" ON)

option(WARNINGS "Enable common compiler warnings (for gcc-compatible compilers and MSVC only)" OFF)
option(WARNINGS_ALL "Enable ALL compiler warnings (for clang and MSVC only)" OFF)
option(WARNINGS_FATAL "Stop compilation on any compiler warning (for gcc-compatible compilers and MSVC only)" OFF)
option(FORCE_LOCAL_LIBS "Compile the built-in versions of SDL2, FreeType, and FLTK instead of using the system-provided ones" OFF)


list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_SOURCE_DIR}/cmake"
)

project(CSE2 LANGUAGES C CXX)

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
	external/lodepng/lodepng.cpp
	external/lodepng/lodepng.h
	src/ArmsItem.cpp
	src/ArmsItem.h
	src/Back.cpp
	src/Back.h
	src/Boss.cpp
	src/Boss.h
	src/BossAlmo1.cpp
	src/BossAlmo1.h
	src/BossAlmo2.cpp
	src/BossAlmo2.h
	src/BossBallos.cpp
	src/BossBallos.h
	src/BossFrog.cpp
	src/BossFrog.h
	src/BossIronH.cpp
	src/BossIronH.h
	src/BossLife.cpp
	src/BossLife.h
	src/BossOhm.cpp
	src/BossOhm.h
	src/BossPress.cpp
	src/BossPress.h
	src/BossTwinD.cpp
	src/BossTwinD.h
	src/BossX.cpp
	src/BossX.h
	src/BulHit.cpp
	src/BulHit.h
	src/Bullet.cpp
	src/Bullet.h
	src/Caret.cpp
	src/Caret.h
	src/CommonDefines.h
	src/Config.cpp
	src/Config.h
	src/Draw.cpp
	src/Draw.h
	src/Ending.cpp
	src/Ending.h
	src/Escape.cpp
	src/Escape.h
	src/Fade.cpp
	src/Fade.h
	src/File.cpp
	src/File.h
	src/Flags.cpp
	src/Flags.h
	src/Flash.cpp
	src/Flash.h
	src/Font.cpp
	src/Font.h
	src/Frame.cpp
	src/Frame.h
	src/Game.cpp
	src/Game.h
	src/Generic.cpp
	src/Generic.h
	src/GenericLoad.cpp
	src/GenericLoad.h
	src/Input.cpp
	src/Input.h
	src/KeyControl.cpp
	src/KeyControl.h
	src/Main.cpp
	src/Main.h
	src/Map.cpp
	src/Map.h
	src/MapName.cpp
	src/MapName.h
	src/MiniMap.cpp
	src/MiniMap.h
	src/MyChar.cpp
	src/MyChar.h
	src/MycHit.cpp
	src/MycHit.h
	src/MycParam.cpp
	src/MycParam.h
	src/NpcAct.h
	src/NpcAct000.cpp
	src/NpcAct020.cpp
	src/NpcAct040.cpp
	src/NpcAct060.cpp
	src/NpcAct080.cpp
	src/NpcAct100.cpp
	src/NpcAct120.cpp
	src/NpcAct140.cpp
	src/NpcAct160.cpp
	src/NpcAct180.cpp
	src/NpcAct200.cpp
	src/NpcAct220.cpp
	src/NpcAct240.cpp
	src/NpcAct260.cpp
	src/NpcAct280.cpp
	src/NpcAct300.cpp
	src/NpcAct320.cpp
	src/NpcAct340.cpp
	src/NpcAct360.cpp
	src/NpcAct380.cpp
	src/NpChar.cpp
	src/NpChar.h
	src/NpcHit.cpp
	src/NpcHit.h
	src/NpcTbl.cpp
	src/NpcTbl.h
	src/Organya.cpp
	src/Organya.h
	src/PixTone.cpp
	src/PixTone.h
	src/Profile.cpp
	src/Profile.h
	src/Random.cpp
	src/Random.h
	src/Resource.cpp
	src/Resource.h
	src/SelStage.cpp
	src/SelStage.h
	src/Shoot.cpp
	src/Shoot.h
	src/Sound.cpp
	src/Sound.h
	src/Stage.cpp
	src/Stage.h
	src/Star.cpp
	src/Star.h
	src/TextScr.cpp
	src/TextScr.h
	src/Triangle.cpp
	src/Triangle.h
	src/ValueView.cpp
	src/ValueView.h
	src/WindowsWrapper.h
	src/Backends/Rendering.h
	src/Backends/Audio/SDL2.cpp
	src/Backends/Audio.h
)

set(RESOURCES
)

target_include_directories(CSE2 PRIVATE "external")
target_compile_definitions(CSE2 PRIVATE LODEPNG_NO_COMPILE_ENCODER LODEPNG_NO_COMPILE_ERROR_TEXT LODEPNG_NO_COMPILE_CPP)

# Handle options
if(JAPANESE)
	set(DATA_DIRECTORY "${ASSETS_DIRECTORY}/data_jp")
	list(APPEND RESOURCES "FONT/NotoSansMonoCJKjp.otf")
	target_compile_definitions(CSE2 PRIVATE JAPANESE)
else()
	set(DATA_DIRECTORY "${ASSETS_DIRECTORY}/data_en")
	list(APPEND RESOURCES "FONT/LiberationMono.ttf")
endif()

if(FIX_BUGS)
	target_compile_definitions(CSE2 PRIVATE FIX_BUGS)
endif()

if(DEBUG_SAVE)
	target_compile_definitions(CSE2 PRIVATE DEBUG_SAVE)
endif()

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

if (WARNINGS_FATAL)
	# HACK : Replace this with CMake provided stuff when possible (when CMake makes avoiding this possible (it isn't currently))

	if (MSVC)
		target_compile_options(CSE2 PRIVATE /WX)
	elseif(COMPILER_IS_GCC_COMPATIBLE)
		target_compile_options(CSE2 PRIVATE -Werror)
	else()
		message(WARNING "Could not activate fatal warnings ! (Unsupported compiler)")
	endif()
endif()

if(RENDERER MATCHES "OpenGL3")
	target_sources(CSE2 PRIVATE "src/Backends/Rendering/OpenGL3.cpp")
elseif(RENDERER MATCHES "SDLTexture")
	target_sources(CSE2 PRIVATE "src/Backends/Rendering/SDLTexture.cpp")
elseif(RENDERER MATCHES "Software")
	target_sources(CSE2 PRIVATE "src/Backends/Rendering/Software.cpp")
else()
	message(FATAL_ERROR "Invalid RENDERER selected")
endif()

if(AUDIO_OGG OR AUDIO_FLAC OR AUDIO_TRACKER OR AUDIO_PXTONE)
	target_sources(CSE2 PRIVATE
		src/ExtraSoundFormats.cpp
		src/ExtraSoundFormats.h
		external/clownaudio/decoder.c
		external/clownaudio/decoder.h
		external/clownaudio/miniaudio.c
		external/clownaudio/miniaudio.h
		external/clownaudio/mixer.c
		external/clownaudio/mixer.h
		external/clownaudio/decoders/common.h
		external/clownaudio/decoders/memory_file.c
		external/clownaudio/decoders/memory_file.h
		external/clownaudio/decoders/misc_utilities.c
		external/clownaudio/decoders/misc_utilities.h
		external/clownaudio/decoders/predecode.c
		external/clownaudio/decoders/predecode.h
		external/clownaudio/decoders/split.c
		external/clownaudio/decoders/split.h
	)

	target_compile_definitions(CSE2 PRIVATE EXTRA_SOUND_FORMATS)
endif()

if(AUDIO_OGG)
	target_sources(CSE2 PRIVATE
		external/clownaudio/decoders/stb_vorbis.c
		external/clownaudio/decoders/stb_vorbis.h
		external/clownaudio/decoders/libs/stb_vorbis.h
	)

	target_compile_definitions(CSE2 PRIVATE USE_STB_VORBIS)
endif()

if(AUDIO_FLAC)
	target_sources(CSE2 PRIVATE
		external/clownaudio/decoders/dr_flac.c
		external/clownaudio/decoders/dr_flac.h
		external/clownaudio/decoders/libs/dr_flac.h
	)

	target_compile_definitions(CSE2 PRIVATE USE_DR_FLAC)
endif()

if(AUDIO_TRACKER)
	target_sources(CSE2 PRIVATE
		external/clownaudio/decoders/libxmp-lite.c
		external/clownaudio/decoders/libxmp-lite.h
	)

	target_compile_definitions(CSE2 PRIVATE USE_LIBXMPLITE)
endif()

if(AUDIO_PXTONE)
	target_sources(CSE2 PRIVATE
		external/clownaudio/decoders/pxtone.c
		external/clownaudio/decoders/pxtone.h
		external/clownaudio/decoders/libs/pxtone/pxtn.h
		external/clownaudio/decoders/libs/pxtone/pxtnDelay.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnDelay.h
		external/clownaudio/decoders/libs/pxtone/pxtnDescriptor.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnDescriptor.h
		external/clownaudio/decoders/libs/pxtone/pxtnError.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnError.h
		external/clownaudio/decoders/libs/pxtone/pxtnEvelist.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnEvelist.h
		external/clownaudio/decoders/libs/pxtone/pxtnMaster.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnMaster.h
		external/clownaudio/decoders/libs/pxtone/pxtnMax.h
		external/clownaudio/decoders/libs/pxtone/pxtnMem.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnMem.h
		external/clownaudio/decoders/libs/pxtone/pxtnOverDrive.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnOverDrive.h
		external/clownaudio/decoders/libs/pxtone/pxtnPulse_Frequency.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnPulse_Frequency.h
		external/clownaudio/decoders/libs/pxtone/pxtnPulse_Noise.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnPulse_Noise.h
		external/clownaudio/decoders/libs/pxtone/pxtnPulse_NoiseBuilder.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnPulse_NoiseBuilder.h
		external/clownaudio/decoders/libs/pxtone/pxtnPulse_Oggv.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnPulse_Oggv.h
		external/clownaudio/decoders/libs/pxtone/pxtnPulse_Oscillator.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnPulse_Oscillator.h
		external/clownaudio/decoders/libs/pxtone/pxtnPulse_PCM.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnPulse_PCM.h
		external/clownaudio/decoders/libs/pxtone/pxtnService.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnService.h
		external/clownaudio/decoders/libs/pxtone/pxtnService_moo.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnText.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnText.h
		external/clownaudio/decoders/libs/pxtone/pxtnUnit.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnUnit.h
		external/clownaudio/decoders/libs/pxtone/pxtnWoice.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnWoice.h
		external/clownaudio/decoders/libs/pxtone/pxtnWoice_io.cpp
		external/clownaudio/decoders/libs/pxtone/pxtnWoicePTV.cpp
		external/clownaudio/decoders/libs/pxtone/pxtoneNoise.cpp
		external/clownaudio/decoders/libs/pxtone/pxtoneNoise.h
		external/clownaudio/decoders/libs/pxtone/shim.cpp
		external/clownaudio/decoders/libs/pxtone/shim.h
	)

	target_compile_definitions(CSE2 PRIVATE USE_PXTONE)
endif()

if(SMOOTH_SPRITE_MOVEMENT)
	target_compile_definitions(CSE2 PRIVATE SMOOTH_SPRITE_MOVEMENT)
endif()

# Make some tweaks if we're targetting Windows
if(WIN32)
	target_sources(CSE2 PRIVATE "${ASSETS_DIRECTORY}/resources/CSE2.rc")
	set_target_properties(CSE2 PROPERTIES WIN32_EXECUTABLE YES)	# Disable the console window
endif()

# Make some tweaks if we're using MSVC
if(MSVC)
	target_compile_definitions(CSE2 PRIVATE _CRT_SECURE_NO_WARNINGS)	# Disable warnings that normally fire up on MSVC when using "unsafe" functions instead of using MSVC's "safe" _s functions
endif()

# Make it so source files are recognized as UTF-8 by MSVC
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Build bin2h externally, so it isn't cross-compiled when CSE2 is (Emscripten, cross-GCC, MinGW on Linux, etc.)
include(ExternalProject)

ExternalProject_Add(bin2h
	SOURCE_DIR "${CMAKE_SOURCE_DIR}/bin2h"
	DOWNLOAD_COMMAND ""
	UPDATE_COMMAND ""
	BUILD_BYPRODUCTS "<INSTALL_DIR>/bin/bin2h"
	CMAKE_ARGS
		-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
		-DCMAKE_BUILD_TYPE=Release
		-DWARNINGS=${WARNINGS}
		-DWARNINGS_ALL=${WARNINGS_ALL}
		-DWARNINGS_FATAL=${WARNINGS_FATAL}
	INSTALL_COMMAND
		${CMAKE_COMMAND} --build . --config Release --target install
)

ExternalProject_Get_Property(bin2h INSTALL_DIR)

add_executable(bin2h_tool IMPORTED)
add_dependencies(bin2h_tool bin2h)
set_target_properties(bin2h_tool PROPERTIES IMPORTED_LOCATION "${INSTALL_DIR}/bin/bin2h")

# Convert resources to header files
foreach(FILENAME IN LISTS RESOURCES)
	set(IN_DIR "${ASSETS_DIRECTORY}/resources")
	set(OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Resource")
	get_filename_component(DIRECTORY "${FILENAME}" DIRECTORY)
	add_custom_command(
		OUTPUT "${OUT_DIR}/${FILENAME}.h"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${OUT_DIR}/${DIRECTORY}"
		COMMAND bin2h_tool "${IN_DIR}/${FILENAME}" "${OUT_DIR}/${FILENAME}.h"
		DEPENDS bin2h_tool "${IN_DIR}/${FILENAME}"
		)
	target_sources(CSE2 PRIVATE "${OUT_DIR}/${FILENAME}.h")
endforeach()

# Force strict C99
set_target_properties(CSE2 PROPERTIES
	C_STANDARD 99
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF
)

# Force strict C++11
set_target_properties(CSE2 PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

# Include directory for third-party software
target_include_directories(CSE2 PRIVATE "external")

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

# Copy data folder to build directory
add_custom_command(TARGET CSE2 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory "${BUILD_DIRECTORY}/data"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${DATA_DIRECTORY}" "${BUILD_DIRECTORY}/data"
)

# Enable link-time optimisation if available
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	if((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
		include(CheckIPOSupported)
		check_ipo_supported(RESULT result)
		if(result)
			set_target_properties(CSE2 PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
		endif()
	endif()
endif()

# Find dependencies

if(NOT FORCE_LOCAL_LIBS)
	find_package(SDL2)
endif()

if(TARGET SDL2::SDL2)
	# CMake-generated config (Arch, vcpkg, Raspbian)
	message(STATUS "Using system SDL2")
	target_link_libraries(CSE2 SDL2::SDL2 SDL2::SDL2main)
elseif(TARGET SDL2::SDL2-static)
	# CMake-generated config (Arch, vcpkg, Raspbian)
	message(STATUS "Using system SDL2")
	target_link_libraries(CSE2 SDL2::SDL2-static SDL2::SDL2main)
elseif(SDL2_FOUND)
	# Autotools-generated config (MSYS2)
	message(STATUS "Using system SDL2")
	target_include_directories(CSE2 PRIVATE ${SDL2_INCLUDE_DIRS})
	target_link_libraries(CSE2 ${SDL2_LIBRARIES})
else()
	# Compile it ourselves
	message(STATUS "Using local SDL2")
	set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
	if(MSVC)
		set(LIBC ON)	# Needed to prevent possible 'symbol already defined' errors
	endif()
	add_subdirectory("external/SDL2" EXCLUDE_FROM_ALL)
	target_link_libraries(CSE2 SDL2-static SDL2main)
endif()

if(NOT FORCE_LOCAL_LIBS)
	find_package(Freetype)
endif()

if(FREETYPE_FOUND)
	message(STATUS "Using system FreeType")
	target_include_directories(CSE2 PRIVATE ${FREETYPE_INCLUDE_DIRS})
	target_link_libraries(CSE2 ${FREETYPE_LIBRARIES})
else()
	# Compile it ourselves
	message(STATUS "Using local FreeType")
	if(FORCE_LOCAL_LIBS)
		set(CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz ON)
		set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB ON)
		set(CMAKE_DISABLE_FIND_PACKAGE_PNG ON)
		set(CMAKE_DISABLE_FIND_PACKAGE_BZip2 ON)
	endif()
	add_subdirectory("external/freetype" EXCLUDE_FROM_ALL)
	target_link_libraries(CSE2 freetype)
endif()

if(RENDERER MATCHES "OpenGL3")
	if(NOT FORCE_LOCAL_LIBS)
		find_package(GLEW)
	endif()

	if(TARGET GLEW::GLEW)
		message(STATUS "Using system GLEW")
		target_link_libraries(CSE2 GLEW::GLEW)
	else()
		# Compile it ourselves
		message(STATUS "Using local GLEW")
		target_sources(CSE2 PRIVATE "external/glew/glew.c" "external/glew/GL/glew.h")
		target_include_directories(CSE2 PRIVATE "external/glew")
	endif()

	find_package(OpenGL REQUIRED)
	target_link_libraries(CSE2 OpenGL::GL)
endif()

if(AUDIO_TRACKER)
	if(NOT FORCE_LOCAL_LIBS)
		find_package(LibXMPLite)
	endif()

	if(libxmp-lite_FOUND)
		message(STATUS "Using system libxmp-lite")
		target_link_libraries(CSE2 libxmp-lite)
	else()
		# Compile it ourselves
		message(STATUS "Using local libxmp-lite")
		add_subdirectory(external/libxmp-lite EXCLUDE_FROM_ALL)
		target_link_libraries(CSE2 libxmp-lite)
	endif()
endif()

##
# DoConfig
##

add_subdirectory("DoConfig")

# Name debug builds "DoConfig_debug", to distinguish them
set_target_properties(DoConfig PROPERTIES DEBUG_OUTPUT_NAME "DoConfig_debug")

# Send executable to the build_en/build_jp directory
set_target_properties(DoConfig PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIRECTORY}
)
