NATIVECC = cc
NATIVECXX = c++

BUILD_DIRECTORY = game
ASSETS_DIRECTORY = assets

# Default options
FIX_BUGS ?= 1
RENDERER ?= Texture
SMOOTH_SPRITE_MOVEMENT ?= 1

ifeq ($(RELEASE), 1)
	CXXFLAGS = -O3 -flto
	LDFLAGS = -s
	FILENAME_DEF = CSE2
	DOCONFIG_FILENAME_DEF = DoConfig
else
	CXXFLAGS = -Og -g3
	FILENAME_DEF = CSE2_debug
	DOCONFIG_FILENAME_DEF = DoConfig_debug
endif

ifeq ($(JAPANESE), 1)
	DATA_DIRECTORY = $(ASSETS_DIRECTORY)/data_jp

	CXXFLAGS += -DJAPANESE
else
	DATA_DIRECTORY = $(ASSETS_DIRECTORY)/data_en
endif

FILENAME ?= $(FILENAME_DEF)
DOCONFIG_FILENAME ?= $(DOCONFIG_FILENAME_DEF)

ifeq ($(FIX_BUGS), 1)
	CXXFLAGS += -DFIX_BUGS
endif

ifeq ($(WINDOWS), 1)
	ifeq ($(CONSOLE), 1)
		CXXFLAGS += -mconsole
	endif

	CXXFLAGS += -DWINDOWS
	LIBS += -lkernel32
endif

ifeq ($(RASPBERRY_PI), 1)
	CXXFLAGS += -DRASPBERRY_PI
endif

CXXFLAGS += `pkg-config sdl2 --cflags` `pkg-config freetype2 --cflags` -MMD -MP -MF $@.d
DEFINES += -DLODEPNG_NO_COMPILE_ENCODER -DLODEPNG_NO_COMPILE_ERROR_TEXT -DLODEPNG_NO_COMPILE_CPP

CFLAGS := $(CXXFLAGS)

CFLAGS += -std=c99
CXXFLAGS += -std=c++11

ifeq ($(STATIC), 1)
	LDFLAGS += -static
	LIBS += `pkg-config sdl2 --libs --static` `pkg-config freetype2 --libs --static` -lfreetype
else
	LIBS += `pkg-config sdl2 --libs` `pkg-config freetype2 --libs`
endif

SOURCES = \
	lodepng/lodepng \
	ArmsItem \
	Back \
	Boss \
	BossAlmo1 \
	BossAlmo2 \
	BossBallos \
	BossFrog \
	BossIronH \
	BossLife \
	BossOhm \
	BossPress \
	BossTwinD \
	BossX \
	BulHit \
	Bullet \
	Caret \
	Config \
	Draw \
	Ending \
	Escape \
	Fade \
	File \
	Flags \
	Flash \
	Font \
	Frame \
	Game \
	Generic \
	GenericLoad \
	Input \
	KeyControl \
	Main \
	Map \
	MapName \
	MiniMap \
	MyChar \
	MycHit \
	MycParam \
	NpcAct000 \
	NpcAct020 \
	NpcAct040 \
	NpcAct060 \
	NpcAct080 \
	NpcAct100 \
	NpcAct120 \
	NpcAct140 \
	NpcAct160 \
	NpcAct180 \
	NpcAct200 \
	NpcAct220 \
	NpcAct240 \
	NpcAct260 \
	NpcAct280 \
	NpcAct300 \
	NpcAct320 \
	NpcAct340 \
	NpChar \
	NpcHit \
	NpcTbl \
	Organya \
	PixTone \
	Profile \
	Resource \
	SelStage \
	Shoot \
	Sound \
	Stage \
	Star \
	TextScr \
	Triangle \
	ValueView

ifneq (,$(filter 1,$(OGG_AUDIO)$(FLAC_AUDIO)$(TRACKER_AUDIO)$(PXTONE_AUDIO)))
	SOURCES += \
		ExtraSoundFormats \
		audio_lib/decoder \
		audio_lib/miniaudio \
		audio_lib/mixer \
		audio_lib/decoders/memory_file \
		audio_lib/decoders/misc_utilities \
		audio_lib/decoders/predecode \
		audio_lib/decoders/split

	DEFINES += -DEXTRA_SOUND_FORMATS
endif

ifeq ($(OGG_AUDIO), 1)
	SOURCES += \
		audio_lib/decoders/stb_vorbis

	DEFINES += -DUSE_STB_VORBIS
endif

ifeq ($(FLAC_AUDIO), 1)
	SOURCES += \
		audio_lib/decoders/dr_flac

	DEFINES += -DUSE_DR_FLAC
endif

ifeq ($(TRACKER_AUDIO), 1)
	SOURCES += \
		audio_lib/decoders/libxmp-lite

	DEFINES += -DUSE_LIBXMPLITE

	CFLAGS += `pkg-config libxmp-lite --cflags`
	CXXFLAGS += `pkg-config libxmp-lite --cflags`

	ifeq ($(STATIC), 1)
		LIBS += `pkg-config libxmp-lite --libs --static`
	else
		LIBS += `pkg-config libxmp-lite --libs`
	endif
endif

ifeq ($(PXTONE_AUDIO), 1)
	SOURCES += \
		audio_lib/decoders/pxtone \
		audio_lib/decoders/libs/pxtone/pxtnDelay \
		audio_lib/decoders/libs/pxtone/pxtnDescriptor \
		audio_lib/decoders/libs/pxtone/pxtnError \
		audio_lib/decoders/libs/pxtone/pxtnEvelist \
		audio_lib/decoders/libs/pxtone/pxtnMaster \
		audio_lib/decoders/libs/pxtone/pxtnMem \
		audio_lib/decoders/libs/pxtone/pxtnOverDrive \
		audio_lib/decoders/libs/pxtone/pxtnPulse_Frequency \
		audio_lib/decoders/libs/pxtone/pxtnPulse_Noise \
		audio_lib/decoders/libs/pxtone/pxtnPulse_NoiseBuilder \
		audio_lib/decoders/libs/pxtone/pxtnPulse_Oggv \
		audio_lib/decoders/libs/pxtone/pxtnPulse_Oscillator \
		audio_lib/decoders/libs/pxtone/pxtnPulse_PCM \
		audio_lib/decoders/libs/pxtone/pxtnService \
		audio_lib/decoders/libs/pxtone/pxtnService_moo \
		audio_lib/decoders/libs/pxtone/pxtnText \
		audio_lib/decoders/libs/pxtone/pxtnUnit \
		audio_lib/decoders/libs/pxtone/pxtnWoice \
		audio_lib/decoders/libs/pxtone/pxtnWoice_io \
		audio_lib/decoders/libs/pxtone/pxtnWoicePTV \
		audio_lib/decoders/libs/pxtone/pxtoneNoise \
		audio_lib/decoders/libs/pxtone/shim

	DEFINES += -DUSE_PXTONE
endif

ifeq ($(SMOOTH_SPRITE_MOVEMENT), 1)
	DEFINES += -DSMOOTH_SPRITE_MOVEMENT
endif

RESOURCES =

ifeq ($(JAPANESE), 1)
	RESOURCES += FONT/msgothic.ttc
else
	ifneq ($(WINDOWS), 1)
		RESOURCES += FONT/cour.ttf
	endif
endif

ifeq ($(RENDERER), Texture)
	SOURCES += Backends/Rendering/SDLTexture
else ifeq ($(RENDERER), Surface)
	SOURCES += Backends/Rendering/Software
else ifeq ($(RENDERER), Software)
	SOURCES += Backends/Rendering/Software
else
	@echo Invalid RENDERER selected; this build will fail
endif

OBJECTS = $(addprefix obj/$(FILENAME)/, $(addsuffix .o, $(SOURCES)))
DEPENDENCIES = $(addprefix obj/$(FILENAME)/, $(addsuffix .o.d, $(SOURCES)))

ifeq ($(WINDOWS), 1)
	OBJECTS += obj/$(FILENAME)/win_icon.o
endif

all: $(BUILD_DIRECTORY)/$(FILENAME) $(BUILD_DIRECTORY)/data $(BUILD_DIRECTORY)/$(DOCONFIG_FILENAME)
	@echo Finished

$(BUILD_DIRECTORY)/data: $(DATA_DIRECTORY)
	@mkdir -p $(@D)
	@rm -rf $(BUILD_DIRECTORY)/data
	@cp -r $(DATA_DIRECTORY) $(BUILD_DIRECTORY)/data

$(BUILD_DIRECTORY)/$(FILENAME): $(OBJECTS)
	@mkdir -p $(@D)
	@echo Linking $@
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

obj/$(FILENAME)/%.o: src/%.c
	@mkdir -p $(@D)
	@echo Compiling $<
	@$(CC) $(CFLAGS) $(DEFINES) $< -o $@ -c

obj/$(FILENAME)/%.o: src/%.cpp
	@mkdir -p $(@D)
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) $(DEFINES) $< -o $@ -c

obj/$(FILENAME)/Resource.o: src/Resource.cpp $(addprefix src/Resource/, $(addsuffix .h, $(RESOURCES)))
	@mkdir -p $(@D)
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) $(DEFINES) $< -o $@ -c

src/Resource/%.h: $(ASSETS_DIRECTORY)/resources/% obj/bin2h
	@mkdir -p $(@D)
	@echo Converting $<
	@obj/bin2h $< $@

obj/bin2h: bin2h/bin2h.c
	@mkdir -p $(@D)
	@echo Compiling $^
	@$(NATIVECC) -O3 -s -std=c90 -Wall -Wextra -pedantic $^ -o $@

include $(wildcard $(DEPENDENCIES))

obj/$(FILENAME)/win_icon.o: $(ASSETS_DIRECTORY)/resources/ICON/ICON.rc $(ASSETS_DIRECTORY)/resources/ICON/0.ico $(ASSETS_DIRECTORY)/resources/ICON/ICON_MINI.ico
	@mkdir -p $(@D)
	@windres $< $@

$(BUILD_DIRECTORY)/$(DOCONFIG_FILENAME): DoConfig/DoConfig.cpp
	@mkdir -p $(@D)
	@echo Linking $@
ifeq ($(STATIC), 1)
	@$(CXX) -O3 -s -std=c++98 -static $^ -o $@ `fltk-config --cxxflags --libs --ldstaticflags`
else
	@$(CXX) -O3 -s -std=c++98 $^ -o $@ `fltk-config --cxxflags --libs --ldflags`
endif

# TODO
clean:
	@rm -rf obj
