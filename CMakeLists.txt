cmake_minimum_required(VERSION 3.7.2)

if((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
	cmake_policy(SET CMP0069 NEW)
endif()

set(BUILD_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/game")
set(ASSETS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")

option(JAPANESE "Enable the Japanese-language build" OFF)
option(FIX_BUGS "Fix certain bugs (see src/Bug Fixes.txt)" OFF)
option(NONPORTABLE "Enable bits of code that aren't portable, but are what the original game used" OFF)
option(FORCE_LOCAL_LIBS "Compile the built-in versions of SDL2, FreeType, and FLTK instead of using the system-provided ones" OFF)

project(CSE2 LANGUAGES C CXX)

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
	src/SoftwareMixer.cpp
	src/SoftwareMixer.h
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
	src/MainLoop.cpp
	src/MainLoop.h
)

set(RESOURCES
	BITMAP/Credit01.bmp
	BITMAP/Credit02.bmp
	BITMAP/Credit03.bmp
	BITMAP/Credit04.bmp
	BITMAP/Credit05.bmp
	BITMAP/Credit06.bmp
	BITMAP/Credit07.bmp
	BITMAP/Credit08.bmp
	BITMAP/Credit09.bmp
	BITMAP/Credit10.bmp
	BITMAP/Credit11.bmp
	BITMAP/Credit12.bmp
	BITMAP/Credit14.bmp
	BITMAP/Credit15.bmp
	BITMAP/Credit16.bmp
	BITMAP/Credit17.bmp
	BITMAP/Credit18.bmp
	CURSOR/CURSOR_IKA.bmp
	CURSOR/CURSOR_NORMAL.bmp
	ORG/Access.org
	ORG/Anzen.org
	ORG/Balcony.org
	ORG/Ballos.org
	ORG/BreakDown.org
	ORG/Cemetery.org
	ORG/Curly.org
	ORG/Dr.org
	ORG/Ending.org
	ORG/Escape.org
	ORG/Fanfale1.org
	ORG/Fanfale2.org
	ORG/Fanfale3.org
	ORG/FireEye.org
	ORG/Gameover.org
	ORG/Ginsuke.org
	ORG/Grand.org
	ORG/Gravity.org
	ORG/Hell.org
	ORG/ironH.org
	ORG/Jenka.org
	ORG/Jenka2.org
	ORG/Kodou.org
	ORG/LastBtl3.org
	ORG/LastBtl.org
	ORG/LastCave.org
	ORG/Marine.org
	ORG/Maze.org
	ORG/MDown2.org
	ORG/Mura.org
	ORG/Oside.org
	ORG/Plant.org
	ORG/quiet.org
	ORG/Requiem.org
	ORG/Toroko.org
	ORG/Vivi.org
	ORG/Wanpak2.org
	ORG/Wanpaku.org
	ORG/Weed.org
	ORG/White.org
	ORG/XXXX.org
	ORG/Zonbie.org
	WAVE/Wave.dat
)

# Handle options
if(JAPANESE)
	set(DATA_DIRECTORY "${ASSETS_DIRECTORY}/data_jp")
	list(APPEND RESOURCES "BITMAP/pixel_jp.bmp")
	target_compile_definitions(CSE2 PRIVATE JAPANESE)
else()
	set(DATA_DIRECTORY "${ASSETS_DIRECTORY}/data_en")
	list(APPEND RESOURCES "BITMAP/pixel.bmp")
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

# Make some tweaks if we're targetting Windows
if(WIN32)
	target_sources(CSE2 PRIVATE "${ASSETS_DIRECTORY}/resources/ICON/ICON.rc")
	target_compile_definitions(CSE2 PRIVATE WINDOWS)
	set_target_properties(CSE2 PROPERTIES WIN32_EXECUTABLE YES)	# Disable the console window
else()
	list(APPEND RESOURCES "ICON/ICON_MINI.bmp")
endif()

# Make some tweaks if we're using MSVC
if(MSVC)
	target_compile_definitions(CSE2 PRIVATE _CRT_SECURE_NO_WARNINGS)	# Shut up those stupid warnings
endif()

# Do some setup if we're targetting Emscripten
if(EMSCRIPTEN)
	target_compile_options(CSE2 PRIVATE "SHELL:-s USE_SDL=2" "SHELL:-s USE_FREETYPE=1")
	target_link_libraries(CSE2 PRIVATE "-s USE_SDL=2 -s USE_FREETYPE=1 --preload-file ${ASSETS_DIRECTORY}/emscripten/fps@/ --preload-file ${DATA_DIRECTORY}@/data --preload-file ${ASSETS_DIRECTORY}/emscripten/Config.dat@/ -s ALLOW_MEMORY_GROWTH=1 --no-heap-copy")
endif()

# Decide the embedded font
if(JAPANESE)
	list(APPEND RESOURCES "FONT/msgothic.ttc")
elseif(NOT WIN32)
	list(APPEND RESOURCES "FONT/cour.ttf")
endif()

# Build bin2h externally, so it isn't cross-compiled when CSE2 is (Emscripten)
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

# Copy data folder to build directory
if(NOT EMSCRIPTEN)
	add_custom_command(TARGET CSE2 POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E remove_directory "${BUILD_DIRECTORY}/data"
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${DATA_DIRECTORY}" "${BUILD_DIRECTORY}/data"
	)
endif()

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

if(NOT EMSCRIPTEN)
	if(NOT FORCE_LOCAL_LIBS)
		find_package(SDL2)
	endif()

	if(TARGET SDL2::SDL2)
		# CMake-generated config (Arch, vcpkg, Raspbian)
		message(STATUS "Using system SDL2")
		target_link_libraries(CSE2 SDL2::SDL2 SDL2::SDL2main)
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
endif()


##
# DoConfig
##

if(NOT EMSCRIPTEN)
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
endif()
