cmake_minimum_required(VERSION 3.7.2)

if ((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
	cmake_policy(SET CMP0069 NEW)
endif()

option(STATIC "Produce a statically-linked executable (good for Windows builds, so you don't need to bundle DLL files)" OFF)
option(JAPANESE "Enable the Japanese-language build" OFF)
option(FIX_BUGS "Fix certain bugs (see src/Bug Fixes.txt)" OFF)
option(NONPORTABLE "Enable bits of code that aren't portable, but are what the original game used" OFF)

# Default to Release build
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

project(CSE2 LANGUAGES C CXX)

# Force strict C90
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Force strict C++98
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(CSE2
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
	BITMAP/CREDIT01.bmp
	BITMAP/CREDIT02.bmp
	BITMAP/CREDIT03.bmp
	BITMAP/CREDIT04.bmp
	BITMAP/CREDIT05.bmp
	BITMAP/CREDIT06.bmp
	BITMAP/CREDIT07.bmp
	BITMAP/CREDIT08.bmp
	BITMAP/CREDIT09.bmp
	BITMAP/CREDIT10.bmp
	BITMAP/CREDIT11.bmp
	BITMAP/CREDIT12.bmp
	BITMAP/CREDIT14.bmp
	BITMAP/CREDIT15.bmp
	BITMAP/CREDIT16.bmp
	BITMAP/CREDIT17.bmp
	BITMAP/CREDIT18.bmp
	CURSOR/CURSOR_IKA.bmp
	CURSOR/CURSOR_NORMAL.bmp
	ORG/ACCESS.org
	ORG/ANZEN.org
	ORG/BALCONY.org
	ORG/BALLOS.org
	ORG/BDOWN.org
	ORG/CEMETERY.org
	ORG/CURLY.org
	ORG/DR.org
	ORG/ENDING.org
	ORG/ESCAPE.org
	ORG/FANFALE1.org
	ORG/FANFALE2.org
	ORG/FANFALE3.org
	ORG/FIREEYE.org
	ORG/GAMEOVER.org
	ORG/GINSUKE.org
	ORG/GRAND.org
	ORG/GRAVITY.org
	ORG/HELL.org
	ORG/IRONH.org
	ORG/JENKA.org
	ORG/JENKA2.org
	ORG/KODOU.org
	ORG/LASTBT3.org
	ORG/LASTBTL.org
	ORG/LASTCAVE.org
	ORG/MARINE.org
	ORG/MAZE.org
	ORG/MDOWN2.org
	ORG/MURA.org
	ORG/OSIDE.org
	ORG/PLANT.org
	ORG/QUIET.org
	ORG/REQUIEM.org
	ORG/TOROKO.org
	ORG/VIVI.org
	ORG/WANPAK2.org
	ORG/WANPAKU.org
	ORG/WEED.org
	ORG/WHITE.org
	ORG/XXXX.org
	ORG/ZONBIE.org
	WAVE/WAVE100
)

# Handle options
if (JAPANESE)
	list(APPEND RESOURCES "BITMAP/PIXEL_JP.bmp")
	target_compile_definitions(CSE2 PRIVATE JAPANESE)
else()
	list(APPEND RESOURCES "BITMAP/PIXEL.bmp")
endif()

if (FIX_BUGS)
	target_compile_definitions(CSE2 PRIVATE FIX_BUGS)
endif()

include(CheckIncludeFile)
CHECK_INCLUDE_FILE(/opt/vc/include/bcm_host.h RASPBERRY_PI)
if (RASPBERRY_PI)
	target_compile_definitions(CSE2 PRIVATE RASPBERRY_PI)
endif()

if (NONPORTABLE)
	target_compile_definitions(CSE2 PRIVATE NONPORTABLE)
endif()

# Make some tweaks if we're targetting Windows
if (WIN32)
	target_sources(CSE2 PRIVATE "res/ICON/ICON.rc")
	target_compile_definitions(CSE2 PRIVATE WINDOWS)
	set_target_properties(CSE2 PROPERTIES WIN32_EXECUTABLE YES)	# Disable the console window
else()
	list(APPEND RESOURCES "ICON/ICON_MINI.bmp")
endif()

# Magic to convert resources to header files
add_executable(bin2h src/misc/bin2h.c)
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

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	if ((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
		# Enable link-time optimisation if available
		include(CheckIPOSupported)
		check_ipo_supported(RESULT result)
		if (result)
			set_target_properties(CSE2 PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
		endif()
	endif()
endif()

if (MSVC)
	# Find dependencies

	# In MSVC we just use our own provided copy of SDL2 and FreeType
	set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/msvc/SDL2/include")
	set(FREETYPE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/msvc/freetype/include")

	if (CMAKE_CL_64)
		set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/msvc/SDL2/lib/x64/SDL2.lib;${CMAKE_CURRENT_SOURCE_DIR}/msvc/SDL2/lib/x64/SDL2main.lib")
		set(FREETYPE_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/msvc/freetype/win64/freetype.lib")
	else()
		set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/msvc/SDL2/lib/x86/SDL2.lib;${CMAKE_CURRENT_SOURCE_DIR}/msvc/SDL2/lib/x86/SDL2main.lib")
		set(FREETYPE_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/msvc/freetype/win32/freetype.lib")
	endif()

	# Static-link the CRT, so we don't need to lug around 10 different DLLs
	foreach(FLAG_VAR
			CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
			CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		if(${FLAG_VAR} MATCHES "/MD")
			string(REGEX REPLACE "/MD" "/MT" ${FLAG_VAR} "${${FLAG_VAR}}")
		endif()
	endforeach()

	# Work around MSVC2003's lack of stdint.h
	include(CheckIncludeFiles)
	check_include_files(stdint.h STDINT_EXISTS)
	if (NOT STDINT_EXISTS)
		target_include_directories(CSE2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/msvc/msvc2003")
	endif()
else()
	# Find dependencies
	if (STATIC)
		# Fall back on pkg-config, since cmake's static support sucks
		find_package(PkgConfig REQUIRED)

		pkg_check_modules(SDL2 REQUIRED sdl2)
		set(SDL2_INCLUDE_DIRS ${SDL2_STATIC_INCLUDE_DIRS})
		set(SDL2_CFLAGS_OTHER ${SDL2_STATIC_CFLAGS_OTHER})
		set(SDL2_LIBRARIES ${SDL2_STATIC_LIBRARIES})

		pkg_check_modules(FREETYPE REQUIRED freetype2)
		set(FREETYPE_INCLUDE_DIRS ${FREETYPE_STATIC_INCLUDE_DIRS})
		set(FREETYPE_CFLAGS_OTHER ${FREETYPE_STATIC_CFLAGS_OTHER})
		set(FREETYPE_LIBRARIES ${FREETYPE_STATIC_LIBRARIES})

		target_link_libraries(CSE2 -static)
	else()
		# SDL2 has no standard way of being used by cmake, so avoid
		# that mess entirely and just use pkg-config instead
		find_package(PkgConfig REQUIRED)
		pkg_check_modules(SDL2 REQUIRED sdl2)

		find_package(Freetype REQUIRED)
	endif()
endif()

target_include_directories(CSE2 PRIVATE ${SDL2_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
target_compile_options(CSE2 PRIVATE ${SDL2_CFLAGS_OTHER} ${FREETYPE_CFLAGS_OTHER})
target_link_libraries(CSE2 ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})
