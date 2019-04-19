# Defaults
FIX_BUGS ?= 1
EXTRA_MUSIC_FORMATS ?= 1

ifeq ($(RELEASE), 1)
	CXXFLAGS = -O3 -flto
	LDFLAGS = -s
	FILENAME_DEF = release
else
	CXXFLAGS = -O0 -g
	FILENAME_DEF = debug
endif

ifeq ($(JAPANESE), 1)
	BUILD_DIR = build_jp

	CXXFLAGS += -DJAPANESE

	ifeq ($(RELEASE), 1)
		FILENAME_DEF = releasejp
	else
		FILENAME_DEF = debugjp
	endif
else
	BUILD_DIR = build_en
endif

FILENAME ?= $(FILENAME_DEF)

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

ifeq ($(EXTRA_MUSIC_FORMATS), 1)
SOURCES += \
	OtherMusicFormats \
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
	audio_lib/decoders/libs/pxtone/shim \
	audio_lib/decoders/dr_flac \
	audio_lib/decoders/memory_file \
	audio_lib/decoders/misc_utilities \
	audio_lib/decoders/predecode \
	audio_lib/decoders/pxtone \
	audio_lib/decoders/split \
	audio_lib/decoders/stb_vorbis \
	audio_lib/decoder \
	audio_lib/miniaudio \
	audio_lib/mixer

DEFINES += -DEXTRA_MUSIC_FORMATS -DUSE_STB_VORBIS -DUSE_DR_FLAC -DUSE_PXTONE
endif

RESOURCES =

OBJECTS = $(addprefix obj/$(FILENAME)/, $(addsuffix .o, $(SOURCES)))
DEPENDENCIES = $(addprefix obj/$(FILENAME)/, $(addsuffix .o.d, $(SOURCES)))

ifeq ($(WINDOWS), 1)
	OBJECTS += obj/$(FILENAME)/win_icon.o
endif

all: $(BUILD_DIR)/$(FILENAME)

$(BUILD_DIR)/$(FILENAME): $(OBJECTS)
	@mkdir -p $(@D)
	@echo Linking
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)
	@echo Finished compiling: $@

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

src/Resource/%.h: res/% obj/bin2h
	@mkdir -p $(@D)
	@echo Converting $<
	@obj/bin2h $< $@

obj/bin2h: src/misc/bin2h.c
	@mkdir -p $(@D)
	@echo Compiling $^
	@$(CC) -O3 -s -std=c90 $^ -o $@

include $(wildcard $(DEPENDENCIES))

obj/$(FILENAME)/win_icon.o: res/ICON/ICON.rc res/ICON/0.ico res/ICON/ICON_MINI.ico
	@mkdir -p $(@D)
	@windres $< $@

# TODO
clean:
	@rm -rf obj
