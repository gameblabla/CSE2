cmake_minimum_required(VERSION 3.12)


#############
# Constants #
#############

set(ASSETS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")


###########
# Options #
###########

option(JAPANESE "Enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)" OFF)
option(FIX_BUGS "Fix various bugs in the game" OFF)
option(DEBUG_SAVE "Re-enable the ability to drag-and-drop save files onto the window" OFF)
set(BACKEND_RENDERER "SDLTexture" CACHE STRING "Which renderer the game should use: 'OpenGL3' for an OpenGL 3.2 renderer, 'OpenGLES2' for an OpenGL ES 2.0 renderer, 'SDLTexture' for SDL2's hardware-accelerated Texture API, 'SDLSurface' for SDL2's software-rendered Surface API, or 'Software' for a handwritten software renderer")
set(BACKEND_AUDIO "SDL2" CACHE STRING "Which audio backend the game should use: 'SDL2' or 'miniaudio'")

option(LTO "Enable link-time optimisation" OFF)
option(MSVC_LINK_STATIC_RUNTIME "Link the static MSVC runtime library" OFF)

option(FORCE_LOCAL_LIBS "Compile the built-in versions of SDL2, FreeType, and FLTK instead of using the system-provided ones" OFF)


#########
# Setup #
#########

project(CSE2 LANGUAGES C CXX)

add_executable(CSE2 WIN32
	"${ASSETS_DIRECTORY}/resources/CSE2.rc"
	"src/ArmsItem.cpp"
	"src/ArmsItem.h"
	"src/Back.cpp"
	"src/Back.h"
	"src/Bitmap.cpp"
	"src/Bitmap.h"
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
	"src/Draw.cpp"
	"src/Draw.h"
	"src/Ending.cpp"
	"src/Ending.h"
	"src/Escape.cpp"
	"src/Escape.h"
	"src/Fade.cpp"
	"src/Fade.h"
	"src/File.cpp"
	"src/File.h"
	"src/Flags.cpp"
	"src/Flags.h"
	"src/Flash.cpp"
	"src/Flash.h"
	"src/Font.cpp"
	"src/Font.h"
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
	"src/Random.cpp"
	"src/Random.h"
	"src/Resource.cpp"
	"src/Resource.h"
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
	"src/Backends/Rendering.h"
	"src/Backends/Audio.h"
)

set(RESOURCES
	"BITMAP/Credit01.bmp"
	"BITMAP/Credit02.bmp"
	"BITMAP/Credit03.bmp"
	"BITMAP/Credit04.bmp"
	"BITMAP/Credit05.bmp"
	"BITMAP/Credit06.bmp"
	"BITMAP/Credit07.bmp"
	"BITMAP/Credit08.bmp"
	"BITMAP/Credit09.bmp"
	"BITMAP/Credit10.bmp"
	"BITMAP/Credit11.bmp"
	"BITMAP/Credit12.bmp"
	"BITMAP/Credit14.bmp"
	"BITMAP/Credit15.bmp"
	"BITMAP/Credit16.bmp"
	"BITMAP/Credit17.bmp"
	"BITMAP/Credit18.bmp"
	"CURSOR/CURSOR_IKA.bmp"
	"CURSOR/CURSOR_NORMAL.bmp"
	"ORG/Access.org"
	"ORG/Anzen.org"
	"ORG/Balcony.org"
	"ORG/Ballos.org"
	"ORG/BreakDown.org"
	"ORG/Cemetery.org"
	"ORG/Curly.org"
	"ORG/Dr.org"
	"ORG/Ending.org"
	"ORG/Escape.org"
	"ORG/Fanfale1.org"
	"ORG/Fanfale2.org"
	"ORG/Fanfale3.org"
	"ORG/FireEye.org"
	"ORG/Gameover.org"
	"ORG/Ginsuke.org"
	"ORG/Grand.org"
	"ORG/Gravity.org"
	"ORG/Hell.org"
	"ORG/ironH.org"
	"ORG/Jenka.org"
	"ORG/Jenka2.org"
	"ORG/Kodou.org"
	"ORG/LastBtl3.org"
	"ORG/LastBtl.org"
	"ORG/LastCave.org"
	"ORG/Marine.org"
	"ORG/Maze.org"
	"ORG/MDown2.org"
	"ORG/Mura.org"
	"ORG/Oside.org"
	"ORG/Plant.org"
	"ORG/quiet.org"
	"ORG/Requiem.org"
	"ORG/Toroko.org"
	"ORG/Vivi.org"
	"ORG/Wanpak2.org"
	"ORG/Wanpaku.org"
	"ORG/Weed.org"
	"ORG/White.org"
	"ORG/XXXX.org"
	"ORG/Zonbie.org"
	"WAVE/Wave.dat"
)

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_SOURCE_DIR}/cmake"
)


###################
# Option handling #
###################

if(JAPANESE)
	set(BUILD_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game_japanese")
	list(APPEND RESOURCES "BITMAP/pixel_jp.bmp")
	target_compile_definitions(CSE2 PRIVATE JAPANESE)
else()
	set(BUILD_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game_english")
	list(APPEND RESOURCES "BITMAP/pixel.bmp")
endif()

if(FIX_BUGS)
	target_compile_definitions(CSE2 PRIVATE FIX_BUGS)
endif()

if(DEBUG_SAVE)
	target_compile_definitions(CSE2 PRIVATE DEBUG_SAVE)
endif()

if(LTO)
	include(CheckIPOSupported)

	check_ipo_supported(RESULT result)

	if(result)
		set_target_properties(CSE2 PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
	endif()
endif()

# This is messy as hell, and has been replaced by CMAKE_MSVC_RUNTIME_LIBRARY,
# but that's a very recent CMake addition, so we're still doing it this way for now
if(MSVC AND MSVC_LINK_STATIC_RUNTIME)
	# Statically-link the CRT (vcpkg static libs do this)
	foreach(flag_var CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		if(${flag_var} MATCHES "/MD")
			string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
		endif()
	endforeach()
endif()

if(BACKEND_RENDERER MATCHES "OpenGL3")
	target_sources(CSE2 PRIVATE "src/Backends/Rendering/OpenGL3.cpp")
elseif(BACKEND_RENDERER MATCHES "OpenGLES2")
	target_sources(CSE2 PRIVATE "src/Backends/Rendering/OpenGLES2.cpp")
elseif(BACKEND_RENDERER MATCHES "SDLTexture")
	target_sources(CSE2 PRIVATE "src/Backends/Rendering/SDLTexture.cpp")
elseif(BACKEND_RENDERER MATCHES "SDLSurface")
	target_sources(CSE2 PRIVATE "src/Backends/Rendering/SDLSurface.cpp")
elseif(BACKEND_RENDERER MATCHES "Software")
	target_sources(CSE2 PRIVATE "src/Backends/Rendering/Software.cpp")
else()
	message(FATAL_ERROR "Invalid BACKEND_RENDERER selected")
endif()

if(BACKEND_AUDIO MATCHES "SDL2")
	target_sources(CSE2 PRIVATE "src/Backends/Audio/SDL2.cpp" "src/Backends/Audio/SoftwareMixer.cpp")
elseif(BACKEND_AUDIO MATCHES "miniaudio")
	target_sources(CSE2 PRIVATE "src/Backends/Audio/miniaudio.cpp" "src/Backends/Audio/SoftwareMixer.cpp")

	# Link libdl, libm, and libpthread
	include(CheckLibraryExists)

	check_library_exists(m pow "" LIBM)
	if(LIBM)
		target_link_libraries(CSE2 PRIVATE m)
	endif()

	check_library_exists(pthread pthread_create "" LIBPTHREAD)
	if(LIBPTHREAD)
		target_link_libraries(CSE2 PRIVATE pthread)
	endif()

	target_link_libraries(CSE2 PRIVATE ${CMAKE_DL_LIBS})
else()
	message(FATAL_ERROR "Invalid BACKEND_AUDIO selected")
endif()


##########
# Tweaks #
##########

# Make some tweaks if we're using MSVC
if(MSVC)
	# Disable warnings that normally fire up on MSVC when using "unsafe" functions instead of using MSVC's "safe" _s functions
	target_compile_definitions(CSE2 PRIVATE _CRT_SECURE_NO_WARNINGS)

	# Make it so source files are recognized as UTF-8 by MSVC
	target_compile_options(CSE2 PRIVATE "/utf-8")
endif()

# On Windows, we use native icons instead
if(NOT WIN32)
	list(APPEND RESOURCES "ICON/ICON_MINI.bmp")
endif()


##################
# Misc. settings #
##################

# Force strict C90
set_target_properties(CSE2 PROPERTIES
	C_STANDARD 90
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


################
# Dependencies #
################

if(NOT FORCE_LOCAL_LIBS)
	find_package(SDL2)
endif()

if(TARGET SDL2::SDL2)
	# CMake-generated config (Arch, vcpkg, Raspbian)
	message(STATUS "Using system SDL2 (CMake, dynamic)")
	target_link_libraries(CSE2 PRIVATE SDL2::SDL2 SDL2::SDL2main)
elseif(TARGET SDL2::SDL2-static)
	# CMake-generated config (Arch, vcpkg, Raspbian)
	message(STATUS "Using system SDL2 (CMake, static)")
	target_link_libraries(CSE2 PRIVATE SDL2::SDL2-static SDL2::SDL2main)
elseif(SDL2_FOUND)
	# Autotools-generated config (MSYS2)
	message(STATUS "Using system SDL2 (Autotools)")
	target_include_directories(CSE2 PRIVATE ${SDL2_INCLUDE_DIRS})
	target_link_libraries(CSE2 PRIVATE ${SDL2_LIBRARIES})
else()
	# Compile it ourselves
	message(STATUS "Using local SDL2")
	set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
	if(MSVC)
		set(LIBC ON)	# Needed to prevent possible 'symbol already defined' errors
	endif()
	add_subdirectory("external/SDL2" EXCLUDE_FROM_ALL)
	target_link_libraries(CSE2 PRIVATE SDL2-static SDL2main)
endif()

if(NOT FORCE_LOCAL_LIBS)
	find_package(Freetype)
endif()

if(FREETYPE_FOUND)
	message(STATUS "Using system FreeType")
	target_include_directories(CSE2 PRIVATE ${FREETYPE_INCLUDE_DIRS})
	target_link_libraries(CSE2 PRIVATE ${FREETYPE_LIBRARIES})
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
	target_link_libraries(CSE2 PRIVATE freetype)
endif()

if(BACKEND_RENDERER MATCHES "OpenGL3")
	target_sources(CSE2 PRIVATE "external/glad/src/glad.c" "external/glad/include/glad/glad.h" "external/glad/include/KHR/khrplatform.h")
	target_include_directories(CSE2 PRIVATE "external/glad/include")

	find_package(OpenGL REQUIRED)
	target_link_libraries(CSE2 PRIVATE OpenGL::GL ${CMAKE_DL_LIBS})
endif()

if(BACKEND_RENDERER MATCHES "OpenGLES2")
	find_package(OpenGLES2 REQUIRED)
	target_include_directories(CSE2 PRIVATE ${OPENGLES2_INCLUDE_DIR})
	target_link_libraries(CSE2 PRIVATE ${OPENGLES2_LIBRARIES})
endif()


#######################
# Resource conversion #
#######################

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


############
# DoConfig #
############

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
