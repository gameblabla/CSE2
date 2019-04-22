cmake_minimum_required(VERSION 3.7.2)

if((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
	cmake_policy(SET CMP0069 NEW)
endif()

option(JAPANESE "Enable the Japanese-language build" OFF)
option(FIX_BUGS "Fix certain bugs (see src/Bug Fixes.txt)" ON)
option(NONPORTABLE "Enable bits of code that aren't portable, but are what the original game used" OFF)
option(EXTRA_MUSIC_FORMATS "Enable support for more music formats, including Ogg Vorbis, FLAC, and Pxtone (not to be confused with Pixtone)" ON)

# Default to Release build
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

project(CSE2 LANGUAGES C CXX)

# Force strict C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Force strict C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
	# Static-link the CRT, so we don't need to lug around 10 different DLLs
	foreach(FLAG_VAR
			CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
			CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		if(${FLAG_VAR} MATCHES "/MD")
			string(REGEX REPLACE "/MD" "/MT" ${FLAG_VAR} "${${FLAG_VAR}}")
		endif()
	endforeach()
endif()

##
# CSE2
##

add_executable(CSE2
	src/lodepng/lodepng.cpp
	src/lodepng/lodepng.h
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
)

set(RESOURCES
)

target_compile_definitions(CSE2 PRIVATE LODEPNG_NO_COMPILE_ENCODER LODEPNG_NO_COMPILE_ERROR_TEXT LODEPNG_NO_COMPILE_CPP)

# Handle options
if(JAPANESE)
	set(BUILD_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build_jp")
	target_compile_definitions(CSE2 PRIVATE JAPANESE)
else()
	set(BUILD_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build_en")
endif()

if(FIX_BUGS)
	target_compile_definitions(CSE2 PRIVATE FIX_BUGS)
endif()

if(EXISTS "/opt/vc/include/bcm_host.h")	# Detect Raspberry Pi
	target_compile_definitions(CSE2 PRIVATE RASPBERRY_PI)
endif()

if(NONPORTABLE)
	target_compile_definitions(CSE2 PRIVATE NONPORTABLE)
endif()

if (EXTRA_MUSIC_FORMATS)
	target_sources(CSE2 PRIVATE
		src/OtherMusicFormats.cpp
		src/OtherMusicFormats.h
		src/audio_lib/decoder.c
		src/audio_lib/decoder.h
		src/audio_lib/miniaudio.c
		src/audio_lib/miniaudio.h
		src/audio_lib/mixer.c
		src/audio_lib/mixer.h
		src/audio_lib/decoders/common.h
		src/audio_lib/decoders/dr_flac.c
		src/audio_lib/decoders/dr_flac.h
		src/audio_lib/decoders/memory_file.c
		src/audio_lib/decoders/memory_file.h
		src/audio_lib/decoders/misc_utilities.c
		src/audio_lib/decoders/misc_utilities.h
		src/audio_lib/decoders/predecode.c
		src/audio_lib/decoders/predecode.h
		src/audio_lib/decoders/pxtone.c
		src/audio_lib/decoders/pxtone.h
		src/audio_lib/decoders/split.c
		src/audio_lib/decoders/split.h
		src/audio_lib/decoders/stb_vorbis.c
		src/audio_lib/decoders/stb_vorbis.h
		src/audio_lib/decoders/libs/dr_flac.h
		src/audio_lib/decoders/libs/stb_vorbis.h
		src/audio_lib/decoders/libs/pxtone/pxtn.h
		src/audio_lib/decoders/libs/pxtone/pxtnDelay.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnDelay.h
		src/audio_lib/decoders/libs/pxtone/pxtnDescriptor.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnDescriptor.h
		src/audio_lib/decoders/libs/pxtone/pxtnError.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnError.h
		src/audio_lib/decoders/libs/pxtone/pxtnEvelist.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnEvelist.h
		src/audio_lib/decoders/libs/pxtone/pxtnMaster.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnMaster.h
		src/audio_lib/decoders/libs/pxtone/pxtnMax.h
		src/audio_lib/decoders/libs/pxtone/pxtnMem.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnMem.h
		src/audio_lib/decoders/libs/pxtone/pxtnOverDrive.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnOverDrive.h
		src/audio_lib/decoders/libs/pxtone/pxtnPulse_Frequency.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnPulse_Frequency.h
		src/audio_lib/decoders/libs/pxtone/pxtnPulse_Noise.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnPulse_Noise.h
		src/audio_lib/decoders/libs/pxtone/pxtnPulse_NoiseBuilder.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnPulse_NoiseBuilder.h
		src/audio_lib/decoders/libs/pxtone/pxtnPulse_Oggv.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnPulse_Oggv.h
		src/audio_lib/decoders/libs/pxtone/pxtnPulse_Oscillator.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnPulse_Oscillator.h
		src/audio_lib/decoders/libs/pxtone/pxtnPulse_PCM.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnPulse_PCM.h
		src/audio_lib/decoders/libs/pxtone/pxtnService.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnService.h
		src/audio_lib/decoders/libs/pxtone/pxtnService_moo.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnText.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnText.h
		src/audio_lib/decoders/libs/pxtone/pxtnUnit.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnUnit.h
		src/audio_lib/decoders/libs/pxtone/pxtnWoice.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnWoice.h
		src/audio_lib/decoders/libs/pxtone/pxtnWoice_io.cpp
		src/audio_lib/decoders/libs/pxtone/pxtnWoicePTV.cpp
		src/audio_lib/decoders/libs/pxtone/pxtoneNoise.cpp
		src/audio_lib/decoders/libs/pxtone/pxtoneNoise.h
		src/audio_lib/decoders/libs/pxtone/shim.cpp
		src/audio_lib/decoders/libs/pxtone/shim.h
	)

	target_compile_definitions(CSE2 PRIVATE EXTRA_MUSIC_FORMATS USE_STB_VORBIS USE_DR_FLAC USE_PXTONE)
endif()

# Make some tweaks if we're targetting Windows
if(WIN32)
	target_sources(CSE2 PRIVATE "res/ICON/ICON.rc")
	target_compile_definitions(CSE2 PRIVATE WINDOWS)
	set_target_properties(CSE2 PROPERTIES WIN32_EXECUTABLE YES)	# Disable the console window
endif()

# Make some tweaks if we're using MSVC
if(MSVC)
	# Work around MSVC2003's lack of stdint.h
	include(CheckIncludeFiles)
	check_include_files(stdint.h STDINT_EXISTS)
	if(NOT STDINT_EXISTS)
		target_include_directories(CSE2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/msvc/msvc2003")
	endif()

	# Shut up those stupid warnings
	target_compile_definitions(CSE2 PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Magic to convert resources to header files
add_executable(bin2h "src/misc/bin2h.c")
if(MSVC)
	target_compile_definitions(bin2h PRIVATE _CRT_SECURE_NO_WARNINGS)	# Shut up those stupid warnings
endif()
foreach(FILENAME IN LISTS RESOURCES)
	set(IN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")
	set(OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Resource")
	get_filename_component(DIRECTORY "${FILENAME}" DIRECTORY)
	add_custom_command(
		OUTPUT "${OUT_DIR}/${FILENAME}.h"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${OUT_DIR}/${DIRECTORY}"
		COMMAND bin2h "${IN_DIR}/${FILENAME}" "${OUT_DIR}/${FILENAME}.h"
		DEPENDS bin2h "${IN_DIR}/${FILENAME}"
		)
	target_sources(CSE2 PRIVATE "${OUT_DIR}/${FILENAME}.h")
endforeach()

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
if(MSVC)
	# In MSVC we just use our own provided copy of SDL2 and FreeType
	target_include_directories(CSE2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/msvc/SDL2/include" "${CMAKE_CURRENT_SOURCE_DIR}/msvc/freetype/include")

	if(CMAKE_CL_64)
		target_link_libraries(CSE2 "${CMAKE_CURRENT_SOURCE_DIR}/msvc/SDL2/lib/x64/SDL2.lib" "${CMAKE_CURRENT_SOURCE_DIR}/msvc/SDL2/lib/x64/SDL2main.lib" "${CMAKE_CURRENT_SOURCE_DIR}/msvc/freetype/win64/freetype.lib")
	else()
		target_link_libraries(CSE2 "${CMAKE_CURRENT_SOURCE_DIR}/msvc/SDL2/lib/x86/SDL2.lib" "${CMAKE_CURRENT_SOURCE_DIR}/msvc/SDL2/lib/x86/SDL2main.lib" "${CMAKE_CURRENT_SOURCE_DIR}/msvc/freetype/win32/freetype.lib")
	endif()
elseif(MSYS)
	target_link_libraries(CSE2 -static)

	# Fall back on pkg-config, since cmake's static support sucks
	find_package(PkgConfig REQUIRED)

	pkg_check_modules(SDL2 REQUIRED sdl2)
	pkg_check_modules(FREETYPE REQUIRED freetype2)

	target_include_directories(CSE2 PRIVATE ${SDL2_STATIC_INCLUDE_DIRS} ${FREETYPE_STATIC_INCLUDE_DIRS})
	target_compile_options(CSE2 PRIVATE ${SDL2_STATIC_CFLAGS_OTHER} ${FREETYPE_STATIC_CFLAGS_OTHER})
	target_link_libraries(CSE2 ${SDL2_STATIC_LIBRARIES} ${FREETYPE_STATIC_LIBRARIES})
else()
	# SDL2 has no standard way of being used by cmake, so avoid
	# that mess entirely and just use pkg-config instead
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(SDL2 REQUIRED sdl2)

	find_package(Freetype REQUIRED)

	target_include_directories(CSE2 PRIVATE ${SDL2_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
	target_compile_options(CSE2 PRIVATE ${SDL2_CFLAGS_OTHER} ${FREETYPE_CFLAGS_OTHER})
	target_link_libraries(CSE2 ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})
endif()

# Send executable to the build_en/build_jp directory
set_target_properties(CSE2 PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIRECTORY}
)

# Also send across SDL2.dll if using MSVC
if(MSVC)
	if(CMAKE_CL_64)
		set(SDL2_DLL_ARCH "x64")
	else()
		set(SDL2_DLL_ARCH "x86")
	endif()

	add_custom_command(TARGET CSE2 POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/msvc/SDL2/lib/${SDL2_DLL_ARCH}/SDL2.dll" "${BUILD_DIRECTORY}/"
	)
endif()

##
# DoConfig
##

add_executable(DoConfig "DoConfig/DoConfig.cpp" "DoConfig/icon.rc")

# Windows tweak
if(WIN32)
	set_target_properties(DoConfig PROPERTIES WIN32_EXECUTABLE YES)	# Disable the console window
endif()

# MSVC tweak
if(MSVC)
	target_compile_definitions(DoConfig PRIVATE _CRT_SECURE_NO_WARNINGS)	# Shut up those stupid warnings
endif()

# Find FLTK
if(MSVC)
	# Use local copy
	target_include_directories(DoConfig PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/msvc/fltk/include")

	if(CMAKE_CL_64)
		target_link_libraries(DoConfig "${CMAKE_CURRENT_SOURCE_DIR}/msvc/fltk/lib/x64/fltk.lib")
	else()
		target_link_libraries(DoConfig "${CMAKE_CURRENT_SOURCE_DIR}/msvc/fltk/lib/x86/fltk.lib")

		# Since the x86 version was built with MSVC2003, we need to enable backwards compatibility on newer compilers
		if((MSVC_VERSION EQUAL 1900) OR (MSVC_VERSION GREATER 1900))
			target_link_libraries(DoConfig "legacy_stdio_definitions.lib")
		endif()
	endif()
elseif(MSYS)
	target_link_libraries(DoConfig -static)

	# Do crazy nonsense to link the static version
	find_package(FLTK REQUIRED)
	target_include_directories(DoConfig PRIVATE ${FLTK_INCLUDE_DIRS})

	if(NOT FLTK_CONFIG_SCRIPT)
		find_program(FLTK_CONFIG_SCRIPT fltk-config)
	endif()

	exec_program(bash ARGS ${FLTK_CONFIG_SCRIPT} --ldstaticflags OUTPUT_VARIABLE FLTK_LDSTATICFLAGS)
	target_link_libraries(DoConfig ${FLTK_LDSTATICFLAGS})
else()
	find_package(FLTK REQUIRED)
	target_include_directories(DoConfig PRIVATE ${FLTK_INCLUDE_DIRS})
	target_link_libraries(DoConfig ${FLTK_LIBRARIES})
endif()

# Send executable to the build_en/build_jp directory
set_target_properties(DoConfig PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIRECTORY}
)

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
