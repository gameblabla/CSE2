cmake_minimum_required(VERSION 3.12)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(CLOWNAUDIO_LIBVORBIS "Enable the libvorbis decoder backend" OFF)
option(CLOWNAUDIO_STB_VORBIS "Enable the stb_vorbis decoder backend" ON)
option(CLOWNAUDIO_DR_MP3 "Enable the dr_mp3 decoder backend" ON)
option(CLOWNAUDIO_LIBOPUS "Enable the libopus decoder backend" OFF)
option(CLOWNAUDIO_LIBFLAC "Enable the libFLAC decoder backend" OFF)
option(CLOWNAUDIO_DR_FLAC "Enable the dr_flac decoder backend" ON)
option(CLOWNAUDIO_DR_WAV "Enable the dr_wav decoder backend" ON)
option(CLOWNAUDIO_LIBSNDFILE "Enable the libsndfile decoder backend" OFF)
option(CLOWNAUDIO_LIBOPENMPT "Enable the libopenmpt decoder backend" OFF)
option(CLOWNAUDIO_LIBXMPLITE "Enable the libxmp-lite decoder backend" OFF)
option(CLOWNAUDIO_PXTONE "Enable the PxTone decoder backend" OFF)
option(CLOWNAUDIO_SNES_SPC "Enable the snes_spc decoder backend" OFF)
option(CLOWNAUDIO_MIXER_ONLY "Disables playback capabilities" OFF)
if(NOT CLOWNAUDIO_MIXER_ONLY)
	set(CLOWNAUDIO_BACKEND "miniaudio" CACHE STRING "Which playback backend to use: supported options are 'miniaudio', 'SDL1', 'SDL2', 'Cubeb', and 'PortAudio'")
endif()

project(clownaudio VERSION 0.2 DESCRIPTION "Stereo audio engine supporting various formats" LANGUAGES C CXX)

add_library(clownaudio
	"include/clownaudio/mixer.h"
	"src/miniaudio.cpp"
	"src/miniaudio.h"
	"src/mixer.cpp"
	"src/decoding/decoder_selector.cpp"
	"src/decoding/decoder_selector.h"
	"src/decoding/predecoder.cpp"
	"src/decoding/predecoder.h"
	"src/decoding/resampled_decoder.cpp"
	"src/decoding/resampled_decoder.h"
	"src/decoding/split_decoder.cpp"
	"src/decoding/split_decoder.h"
	"src/decoding/decoders/common.h"
	"src/decoding/decoders/memory_stream.cpp"
	"src/decoding/decoders/memory_stream.h"
)

if(NOT CLOWNAUDIO_MIXER_ONLY)
	target_sources(clownaudio PRIVATE
		"include/clownaudio/clownaudio.h"
		"include/clownaudio/playback.h"
		"src/clownaudio.cpp"
	)
endif()


####################
# Decoding backends
####################

if(CLOWNAUDIO_LIBVORBIS)
	target_compile_definitions(clownaudio PRIVATE USE_LIBVORBIS)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/libvorbis.cpp"
		"src/decoding/decoders/libvorbis.h"
	)
endif()

if(CLOWNAUDIO_STB_VORBIS)
	target_compile_definitions(clownaudio PRIVATE USE_STB_VORBIS)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/stb_vorbis.cpp"
		"src/decoding/decoders/stb_vorbis.h"
	)

	include(CheckLibraryExists)

	check_library_exists(m exp "" LIBM)
	if(LIBM)
		target_link_libraries(clownaudio PRIVATE m)
		list(APPEND STATIC_LIBS m)
	endif()
endif()

if(CLOWNAUDIO_DR_MP3)
	target_compile_definitions(clownaudio PRIVATE USE_DR_MP3)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/dr_mp3.cpp"
		"src/decoding/decoders/dr_mp3.h"
	)
endif()

if(CLOWNAUDIO_LIBOPUS)
	target_compile_definitions(clownaudio PRIVATE USE_LIBOPUS)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/libopus.cpp"
		"src/decoding/decoders/libopus.h"
	)
endif()

if(CLOWNAUDIO_LIBFLAC)
	target_compile_definitions(clownaudio PRIVATE USE_LIBFLAC)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/libflac.cpp"
		"src/decoding/decoders/libflac.h"
	)
endif()

if(CLOWNAUDIO_DR_FLAC)
	target_compile_definitions(clownaudio PRIVATE USE_DR_FLAC)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/dr_flac.cpp"
		"src/decoding/decoders/dr_flac.h"
		"src/decoding/decoders/libs/dr_flac.h"
	)
endif()

if(CLOWNAUDIO_DR_WAV)
	target_compile_definitions(clownaudio PRIVATE USE_DR_WAV)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/dr_wav.cpp"
		"src/decoding/decoders/dr_wav.h"
		"src/decoding/decoders/libs/dr_wav.h"
	)
endif()

if(CLOWNAUDIO_LIBSNDFILE)
	target_compile_definitions(clownaudio PRIVATE USE_LIBSNDFILE)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/libsndfile.cpp"
		"src/decoding/decoders/libsndfile.h"
	)
endif()

if(CLOWNAUDIO_LIBOPENMPT)
	target_compile_definitions(clownaudio PRIVATE USE_LIBOPENMPT)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/libopenmpt.cpp"
		"src/decoding/decoders/libopenmpt.h"
	)
endif()

if(CLOWNAUDIO_LIBXMPLITE)
	target_compile_definitions(clownaudio PRIVATE USE_LIBXMPLITE)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/libxmp-lite.cpp"
		"src/decoding/decoders/libxmp-lite.h"
	)
endif()

if(CLOWNAUDIO_PXTONE)
	target_compile_definitions(clownaudio PRIVATE USE_PXTONE)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/pxtone.cpp"
		"src/decoding/decoders/pxtone.h"
		"src/decoding/decoders/pxtone_noise.cpp"
		"src/decoding/decoders/pxtone_noise.h"
		"src/decoding/decoders/libs/pxtone/pxtn.h"
		"src/decoding/decoders/libs/pxtone/pxtnDelay.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnDelay.h"
		"src/decoding/decoders/libs/pxtone/pxtnDescriptor.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnDescriptor.h"
		"src/decoding/decoders/libs/pxtone/pxtnError.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnError.h"
		"src/decoding/decoders/libs/pxtone/pxtnEvelist.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnEvelist.h"
		"src/decoding/decoders/libs/pxtone/pxtnMaster.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnMaster.h"
		"src/decoding/decoders/libs/pxtone/pxtnMax.h"
		"src/decoding/decoders/libs/pxtone/pxtnMem.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnMem.h"
		"src/decoding/decoders/libs/pxtone/pxtnOverDrive.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnOverDrive.h"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Frequency.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Frequency.h"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Noise.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Noise.h"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_NoiseBuilder.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_NoiseBuilder.h"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Oggv.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Oggv.h"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Oscillator.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Oscillator.h"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_PCM.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_PCM.h"
		"src/decoding/decoders/libs/pxtone/pxtnService.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnService.h"
		"src/decoding/decoders/libs/pxtone/pxtnService_moo.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnText.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnText.h"
		"src/decoding/decoders/libs/pxtone/pxtnUnit.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnUnit.h"
		"src/decoding/decoders/libs/pxtone/pxtnWoice.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnWoice.h"
		"src/decoding/decoders/libs/pxtone/pxtnWoice_io.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnWoicePTV.cpp"
		"src/decoding/decoders/libs/pxtone/pxtoneNoise.cpp"
		"src/decoding/decoders/libs/pxtone/pxtoneNoise.h"
	)
	list(APPEND STATIC_LIBS stdc++)
endif()

if(CLOWNAUDIO_SNES_SPC)
	target_compile_definitions(clownaudio PRIVATE USE_SNES_SPC)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/snes_spc.cpp"
		"src/decoding/decoders/snes_spc.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_common.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_config.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_endian.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_source.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/dsp.cpp"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/dsp.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC.cpp"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC_misc.cpp"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC_state.cpp"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/spc.cpp"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/spc.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_CPU.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_DSP.cpp"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_DSP.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_Filter.cpp"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_Filter.h"
	)
endif()


####################
# Playback backends
####################

if(NOT CLOWNAUDIO_MIXER_ONLY)
	if(CLOWNAUDIO_BACKEND STREQUAL "miniaudio")
		target_sources(clownaudio PRIVATE "src/playback/miniaudio.cpp")
		target_compile_definitions(clownaudio PRIVATE MINIAUDIO_ENABLE_DEVICE_IO)

		include(CheckLibraryExists)

		check_library_exists(m pow "" LIBM)
		if(LIBM)
			target_link_libraries(clownaudio PRIVATE m)
			list(APPEND STATIC_LIBS m)
		endif()

		check_library_exists(pthread pthread_create "" LIBPTHREAD)
		if(LIBPTHREAD)
			target_link_libraries(clownaudio PRIVATE pthread)
			list(APPEND STATIC_LIBS pthread)
		endif()

		target_link_libraries(clownaudio PRIVATE ${CMAKE_DL_LIBS})
		list(APPEND STATIC_LIBS ${CMAKE_DL_LIBS})
	elseif(CLOWNAUDIO_BACKEND STREQUAL "SDL1")
		target_sources(clownaudio PRIVATE "src/playback/sdl1.cpp")
	elseif(CLOWNAUDIO_BACKEND STREQUAL "SDL2")
		target_sources(clownaudio PRIVATE "src/playback/sdl2.cpp")
	elseif(CLOWNAUDIO_BACKEND STREQUAL "Cubeb")
		target_sources(clownaudio PRIVATE "src/playback/cubeb.cpp")
	elseif(CLOWNAUDIO_BACKEND STREQUAL "PortAudio")
		target_sources(clownaudio PRIVATE "src/playback/portaudio.cpp")
	else()
		message(FATAL_ERROR "Invalid BACKEND selected")
	endif()
endif()


####################
# Find dependencies
####################

# CMake

if(NOT CLOWNAUDIO_MIXER_ONLY)
	if(CLOWNAUDIO_BACKEND STREQUAL "Cubeb")
		find_package(cubeb REQUIRED)

		target_link_libraries(clownaudio PRIVATE cubeb::cubeb)
		list(APPEND STATIC_LIBS cubeb)
	endif()
endif()

# pkg-config

find_package(PkgConfig QUIET)

if(NOT CLOWNAUDIO_MIXER_ONLY)
	if(CLOWNAUDIO_BACKEND STREQUAL "SDL1")
		if (NOT PkgConfig_FOUND)
			message(FATAL_ERROR "pkg-config is needed by SDL1, but it couldn't be found")
		endif()

		pkg_check_modules(sdl REQUIRED IMPORTED_TARGET sdl)
		target_link_libraries(clownaudio PRIVATE PkgConfig::sdl)
		set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} sdl")
	endif()

	if(CLOWNAUDIO_BACKEND STREQUAL "SDL2")
		if (NOT PkgConfig_FOUND)
			message(FATAL_ERROR "pkg-config is needed by SDL2, but it couldn't be found")
		endif()

		pkg_check_modules(sdl2 REQUIRED IMPORTED_TARGET sdl2)
		target_link_libraries(clownaudio PRIVATE PkgConfig::sdl2)
		set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} sdl2")
	endif()

	if(CLOWNAUDIO_BACKEND STREQUAL "PortAudio")
		if (NOT PkgConfig_FOUND)
			message(FATAL_ERROR "pkg-config is needed by PortAudio, but it couldn't be found")
		endif()

		pkg_check_modules(portaudio REQUIRED IMPORTED_TARGET portaudio-2.0)
		target_link_libraries(clownaudio PRIVATE PkgConfig::portaudio)
		set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} portaudio-2.0")
	endif()
endif()

if(CLOWNAUDIO_LIBVORBIS)
	if (NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config is needed by libvorbis, but it couldn't be found")
	endif()

	pkg_check_modules(vorbisfile REQUIRED IMPORTED_TARGET vorbisfile)
	target_link_libraries(clownaudio PRIVATE PkgConfig::vorbisfile)
	set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} vorbisfile")
endif()

if(CLOWNAUDIO_LIBOPUS)
	if (NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config is needed by libopus, but it couldn't be found")
	endif()

	pkg_check_modules(opusfile REQUIRED IMPORTED_TARGET opusfile)
	target_link_libraries(clownaudio PRIVATE PkgConfig::opusfile)
	set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} opusfile")
endif()

if(CLOWNAUDIO_LIBFLAC)
	if (NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config is needed by libflac, but it couldn't be found")
	endif()

	pkg_check_modules(flac REQUIRED IMPORTED_TARGET flac)
	target_link_libraries(clownaudio PRIVATE PkgConfig::flac)
	set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} flac")
endif()

if(CLOWNAUDIO_LIBSNDFILE)
	if (NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config is needed by libsndfile, but it couldn't be found")
	endif()

	pkg_check_modules(sndfile REQUIRED IMPORTED_TARGET sndfile)
	target_link_libraries(clownaudio PRIVATE PkgConfig::sndfile)
	set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} sndfile")
endif()

if(CLOWNAUDIO_LIBOPENMPT)
	if (NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config is needed by libopenmpt, but it couldn't be found")
	endif()

	pkg_check_modules(libopenmpt REQUIRED IMPORTED_TARGET libopenmpt)
	target_link_libraries(clownaudio PRIVATE PkgConfig::libopenmpt)
	set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} libopenmpt")
endif()

if(CLOWNAUDIO_LIBXMPLITE)
	if (PkgConfig_FOUND)
		pkg_check_modules(libxmp-lite IMPORTED_TARGET libxmp-lite)
	endif()

	if(libxmp-lite_FOUND)
		message(STATUS "Using system libxmp-lite")
		target_link_libraries(clownaudio PRIVATE PkgConfig::libxmp-lite)
		set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} libxmp-lite")
	else()
		# Compile it ourselves
		message(STATUS "Using local libxmp-lite")
		target_sources(clownaudio PRIVATE
			"src/decoding/decoders/libs/libxmp-lite/include/libxmp-lite/xmp.h"
			"src/decoding/decoders/libs/libxmp-lite/src/common.h"
			"src/decoding/decoders/libs/libxmp-lite/src/control.c"
			"src/decoding/decoders/libs/libxmp-lite/src/dataio.c"
			"src/decoding/decoders/libs/libxmp-lite/src/effects.c"
			"src/decoding/decoders/libs/libxmp-lite/src/effects.h"
			"src/decoding/decoders/libs/libxmp-lite/src/filter.c"
			"src/decoding/decoders/libs/libxmp-lite/src/format.c"
			"src/decoding/decoders/libs/libxmp-lite/src/format.h"
			"src/decoding/decoders/libs/libxmp-lite/src/hio.c"
			"src/decoding/decoders/libs/libxmp-lite/src/hio.h"
			"src/decoding/decoders/libs/libxmp-lite/src/lfo.c"
			"src/decoding/decoders/libs/libxmp-lite/src/lfo.h"
			"src/decoding/decoders/libs/libxmp-lite/src/list.h"
			"src/decoding/decoders/libs/libxmp-lite/src/load.c"
			"src/decoding/decoders/libs/libxmp-lite/src/load_helpers.c"
			"src/decoding/decoders/libs/libxmp-lite/src/mdataio.h"
			"src/decoding/decoders/libs/libxmp-lite/src/memio.c"
			"src/decoding/decoders/libs/libxmp-lite/src/memio.h"
			"src/decoding/decoders/libs/libxmp-lite/src/mix_all.c"
			"src/decoding/decoders/libs/libxmp-lite/src/mixer.c"
			"src/decoding/decoders/libs/libxmp-lite/src/mixer.h"
			"src/decoding/decoders/libs/libxmp-lite/src/period.c"
			"src/decoding/decoders/libs/libxmp-lite/src/period.h"
			"src/decoding/decoders/libs/libxmp-lite/src/player.c"
			"src/decoding/decoders/libs/libxmp-lite/src/player.h"
			"src/decoding/decoders/libs/libxmp-lite/src/precomp_lut.h"
			"src/decoding/decoders/libs/libxmp-lite/src/read_event.c"
			"src/decoding/decoders/libs/libxmp-lite/src/scan.c"
			"src/decoding/decoders/libs/libxmp-lite/src/smix.c"
			"src/decoding/decoders/libs/libxmp-lite/src/tempfile.h"
			"src/decoding/decoders/libs/libxmp-lite/src/virtual.c"
			"src/decoding/decoders/libs/libxmp-lite/src/virtual.h"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/common.c"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/it.h"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/it_load.c"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/itsex.c"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/loader.h"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/mod.h"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/mod_load.c"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/s3m.h"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/s3m_load.c"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/sample.c"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/xm.h"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/xm_load.c"
		)

		target_include_directories(clownaudio PRIVATE "src/decoding/decoders/libs/libxmp-lite/include/libxmp-lite")
		target_compile_definitions(clownaudio PRIVATE LIBXMP_CORE_PLAYER=1)
	endif()
endif()


####################
# Misc. settings
####################

# Include the public headers
target_include_directories(clownaudio PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# Export symbols
include(GenerateExportHeader)
GENERATE_EXPORT_HEADER(clownaudio EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/export/clownaudio_export.h")
target_include_directories(clownaudio PUBLIC "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/export>")

set_target_properties(clownaudio PROPERTIES C_VISIBILITY_PRESET hidden CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN ON)

# Define public header
if(CLOWNAUDIO_MIXER_ONLY)
	set_target_properties(clownaudio PROPERTIES PUBLIC_HEADER "include/clownaudio/mixer.h;${CMAKE_BINARY_DIR}/export/clownaudio_export.h")
else()
	set_target_properties(clownaudio PROPERTIES PUBLIC_HEADER "include/clownaudio/clownaudio.h;include/clownaudio/playback.h;include/clownaudio/mixer.h;${CMAKE_BINARY_DIR}/export/clownaudio_export.h")
endif()

# Set language standards
set_target_properties(clownaudio PROPERTIES
	C_STANDARD 99
	C_EXTENSIONS OFF
	CXX_STANDARD 11
	CXX_EXTENSIONS OFF
)

# Shut up those stupid MSVC warnings
if(MSVC)
	target_compile_definitions(clownaudio PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Group files by directory when using an IDE
get_target_property(CLOWNAUDIO_SOURCES clownaudio SOURCES) 
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES CLOWNAUDIO_SOURCES)


####################
# Install
####################

include(GNUInstallDirs)

# Install directories
install(TARGETS clownaudio
	EXPORT clownaudioTargets
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/clownaudio"
)

# Install `clownaudioConfig.cmake` and `clownaudioTargets.cmake`
install(EXPORT clownaudioTargets DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/clownaudio")

export(TARGETS clownaudio FILE "clownaudioTargets.cmake")

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"clownaudioConfig.cmake.in"
	"clownaudioConfig.cmake"
	INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/clownaudio"
)

install(FILES "${CMAKE_BINARY_DIR}/clownaudioConfig.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/clownaudio")

# pkg-config
if(DEFINED STATIC_LIBS)
	list(REMOVE_DUPLICATES STATIC_LIBS)
	foreach(ITEM ${STATIC_LIBS})
		set(PKG_CONFIG_STATIC_LIBS "${PKG_CONFIG_STATIC_LIBS} -l${ITEM}")
	endforeach()
endif()

configure_file("clownaudio.pc.in" "clownaudio.pc" @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/clownaudio.pc" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig")
