cmake_minimum_required(VERSION 3.4)
project(libxmp-lite)

set(HEADERS "include/libxmp-lite/xmp.h")

set(SOURCES src/virtual.c
            src/format.c
            src/period.c
            src/player.c
            src/read_event.c
            src/dataio.c
            src/lfo.c
            src/scan.c
            src/control.c
            src/filter.c
            src/effects.c
            src/mixer.c
            src/mix_all.c
            src/load_helpers.c
            src/load.c
            src/hio.c
            src/smix.c
            src/memio.c
            src/loaders/common.c
            src/loaders/itsex.c
            src/loaders/sample.c
            src/loaders/xm_load.c
            src/loaders/mod_load.c
            src/loaders/s3m_load.c
            src/loaders/it_load.c
)

add_library(libxmp-lite ${SOURCES})
target_compile_definitions(libxmp-lite PRIVATE
            LIBXMP_CORE_PLAYER=1
            inline=__inline
            _USE_MATH_DEFINES=1
	    BUILDING_STATIC=1
)

target_include_directories(libxmp-lite PUBLIC include/libxmp-lite)
target_include_directories(libxmp-lite PRIVATE src)

if(MSVC)
	target_compile_definitions(libxmp-lite PRIVATE _CRT_SECURE_NO_WARNINGS=1)
endif()
