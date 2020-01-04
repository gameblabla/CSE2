NATIVECC = cc
NATIVECXX = c++
WINDRES = windres

BUILD_DIRECTORY = game
ASSETS_DIRECTORY = assets

# Default options
FIX_BUGS = 1
RENDERER = SDLTexture
SMOOTH_SPRITE_MOVEMENT ?= 1

ALL_CFLAGS = $(CFLAGS)
ALL_CXXFLAGS = $(CXXFLAGS)
ALL_LDFLAGS = $(LDFLAGS)
ALL_LIBS = $(LIBS)

ifeq ($(WINDOWS), 1)
  EXE_EXTENSION = .exe
endif

ifeq ($(RELEASE), 1)
  ALL_CFLAGS += -O3 -DNDEBUG
  ALL_CXXFLAGS += -O3 -DNDEBUG
  ALL_LDFLAGS += -s
  FILENAME_DEF = CSE2$(EXE_EXTENSION)
else
  ALL_CFLAGS += -Og -ggdb3
  ALL_CXXFLAGS += -Og -ggdb3
  FILENAME_DEF = CSE2_debug$(EXE_EXTENSION)
endif

ifeq ($(JAPANESE), 1)
  DATA_DIRECTORY = $(ASSETS_DIRECTORY)/data_jp

  DEFINES += -DJAPANESE
else
  DATA_DIRECTORY = $(ASSETS_DIRECTORY)/data_en
endif

FILENAME ?= $(FILENAME_DEF)

ifeq ($(FIX_BUGS), 1)
  DEFINES += -DFIX_BUGS
endif

ifeq ($(DEBUG_SAVE), 1)
  DEFINES += -DDEBUG_SAVE
endif

ifeq ($(WARNINGS), 1)
  ALL_CFLAGS += -Wall -Wextra -pedantic
  ALL_CXXFLAGS += -Wall -Wextra -pedantic
endif

ifeq ($(WARNINGS_ALL), 1)
  ifneq ($(findstring clang,$(CXX)),)
    # Use clang-specific flag -Weverything
    ALL_CFLAGS += -Weverything
    ALL_CXXFLAGS += -Weverything
  else
    $(warning Couldn't activate all warnings (unsupported compiler))
  endif
endif

ifeq ($(WARNINGS_FATAL), 1)
  ALL_CFLAGS += -Werror
  ALL_CXXFLAGS += -Werror
endif

ALL_CFLAGS += -std=c99 -MMD -MP -MF $@.d
CSE2_CFLAGS += $(shell pkg-config sdl2 --cflags) $(shell pkg-config freetype2 --cflags) -Iexternal

ALL_CXXFLAGS += -std=c++11 -MMD -MP -MF $@.d
CSE2_CXXFLAGS += $(shell pkg-config sdl2 --cflags) $(shell pkg-config freetype2 --cflags) -Iexternal

DEFINES += -DLODEPNG_NO_COMPILE_ENCODER -DLODEPNG_NO_COMPILE_ERROR_TEXT -DLODEPNG_NO_COMPILE_CPP

ifeq ($(STATIC), 1)
  ALL_LDFLAGS += -static
  CSE2_LIBS += $(shell pkg-config sdl2 --libs --static) $(shell pkg-config freetype2 --libs --static) -lfreetype
else
  CSE2_LIBS += $(shell pkg-config sdl2 --libs) $(shell pkg-config freetype2 --libs)
endif

SOURCES = \
  external/lodepng/lodepng \
  src/ArmsItem \
  src/Back \
  src/Boss \
  src/BossAlmo1 \
  src/BossAlmo2 \
  src/BossBallos \
  src/BossFrog \
  src/BossIronH \
  src/BossLife \
  src/BossOhm \
  src/BossPress \
  src/BossTwinD \
  src/BossX \
  src/BulHit \
  src/Bullet \
  src/Caret \
  src/Config \
  src/Draw \
  src/Ending \
  src/Fade \
  src/File \
  src/Flags \
  src/Flash \
  src/Font \
  src/Frame \
  src/Game \
  src/Generic \
  src/GenericLoad \
  src/Input \
  src/KeyControl \
  src/Main \
  src/Map \
  src/MapName \
  src/MiniMap \
  src/MyChar \
  src/MycHit \
  src/MycParam \
  src/NpcAct000 \
  src/NpcAct020 \
  src/NpcAct040 \
  src/NpcAct060 \
  src/NpcAct080 \
  src/NpcAct100 \
  src/NpcAct120 \
  src/NpcAct140 \
  src/NpcAct160 \
  src/NpcAct180 \
  src/NpcAct200 \
  src/NpcAct220 \
  src/NpcAct240 \
  src/NpcAct260 \
  src/NpcAct280 \
  src/NpcAct300 \
  src/NpcAct320 \
  src/NpcAct340 \
  src/NpcAct360 \
  src/NpcAct380 \
  src/NpChar \
  src/NpcHit \
  src/NpcTbl \
  src/Organya \
  src/Pause \
  src/PixTone \
  src/Profile \
  src/Random \
  src/Resource \
  src/SelStage \
  src/Shoot \
  src/Sound \
  src/Stage \
  src/Star \
  src/TextScr \
  src/Triangle \
  src/ValueView \
  src/Backends/Audio/SDL2

ifneq (,$(filter 1,$(AUDIO_OGG)$(AUDIO_FLAC) $(AUDIO_TRACKER) $(AUDIO_PXTONE)))
  SOURCES += \
    src/ExtraSoundFormats \
    external/clownaudio/decoder \
    external/clownaudio/miniaudio \
    external/clownaudio/mixer \
    external/clownaudio/decoders/memory_file \
    external/clownaudio/decoders/misc_utilities \
    external/clownaudio/decoders/predecode \
    external/clownaudio/decoders/split

  DEFINES += -DEXTRA_SOUND_FORMATS
endif

ifeq ($(AUDIO_OGG), 1)
  SOURCES += \
    external/clownaudio/decoders/stb_vorbis

  DEFINES += -DUSE_STB_VORBIS
endif

ifeq ($(AUDIO_FLAC), 1)
  SOURCES += \
    external/clownaudio/decoders/dr_flac

  DEFINES += -DUSE_DR_FLAC
endif

ifeq ($(AUDIO_TRACKER), 1)
  SOURCES += \
    external/clownaudio/decoders/libxmp-lite

  DEFINES += -DUSE_LIBXMPLITE

  ALL_CFLAGS += $(shell pkg-config libxmp-lite --cflags)
  ALL_CXXFLAGS += $(shell pkg-config libxmp-lite --cflags)

  ifeq ($(STATIC), 1)
    ALL_LIBS += $(shell pkg-config libxmp-lite --libs --static)
  else
    ALL_LIBS += $(shell pkg-config libxmp-lite --libs)
  endif
endif

ifeq ($(AUDIO_PXTONE), 1)
  SOURCES += \
    external/clownaudio/decoders/pxtone \
    external/clownaudio/decoders/libs/pxtone/pxtnDelay \
    external/clownaudio/decoders/libs/pxtone/pxtnDescriptor \
    external/clownaudio/decoders/libs/pxtone/pxtnError \
    external/clownaudio/decoders/libs/pxtone/pxtnEvelist \
    external/clownaudio/decoders/libs/pxtone/pxtnMaster \
    external/clownaudio/decoders/libs/pxtone/pxtnMem \
    external/clownaudio/decoders/libs/pxtone/pxtnOverDrive \
    external/clownaudio/decoders/libs/pxtone/pxtnPulse_Frequency \
    external/clownaudio/decoders/libs/pxtone/pxtnPulse_Noise \
    external/clownaudio/decoders/libs/pxtone/pxtnPulse_NoiseBuilder \
    external/clownaudio/decoders/libs/pxtone/pxtnPulse_Oggv \
    external/clownaudio/decoders/libs/pxtone/pxtnPulse_Oscillator \
    external/clownaudio/decoders/libs/pxtone/pxtnPulse_PCM \
    external/clownaudio/decoders/libs/pxtone/pxtnService \
    external/clownaudio/decoders/libs/pxtone/pxtnService_moo \
    external/clownaudio/decoders/libs/pxtone/pxtnText \
    external/clownaudio/decoders/libs/pxtone/pxtnUnit \
    external/clownaudio/decoders/libs/pxtone/pxtnWoice \
    external/clownaudio/decoders/libs/pxtone/pxtnWoice_io \
    external/clownaudio/decoders/libs/pxtone/pxtnWoicePTV \
    external/clownaudio/decoders/libs/pxtone/pxtoneNoise \
    external/clownaudio/decoders/libs/pxtone/shim

  DEFINES += -DUSE_PXTONE
endif

ifeq ($(SMOOTH_SPRITE_MOVEMENT), 1)
  DEFINES += -DSMOOTH_SPRITE_MOVEMENT
endif

RESOURCES = 

ifeq ($(JAPANESE), 1)
  RESOURCES += FONT/NotoSansMonoCJKjp.otf
else
  RESOURCES += FONT/LiberationMono.ttf
endif

ifeq ($(RENDERER), OpenGL3)
  SOURCES += src/Backends/Rendering/OpenGL3
  CSE2_CFLAGS += $(shell pkg-config glew --cflags)
  CSE2_CXXFLAGS += $(shell pkg-config glew --cflags)

  ifeq ($(STATIC), 1)
    CSE2_CFLAGS += -DGLEW_STATIC
    CSE2_CXXFLAGS += -DGLEW_STATIC
    CSE2_LIBS += $(shell pkg-config glew --libs --static)
  else
    CSE2_LIBS += $(shell pkg-config glew --libs)
  endif

  ifeq ($(WINDOWS), 1)
    CSE2_LIBS += -lopengl32
  else
    CSE2_LIBS += -lGL
  endif
else ifeq ($(RENDERER), SDLTexture)
  SOURCES += src/Backends/Rendering/SDLTexture
else ifeq ($(RENDERER), Software)
  SOURCES += src/Backends/Rendering/Software
else
  $(error Invalid RENDERER selected)
endif

OBJECTS = $(addprefix obj/$(FILENAME)/, $(addsuffix .o, $(SOURCES)))
DEPENDENCIES = $(addprefix obj/$(FILENAME)/, $(addsuffix .o.d, $(SOURCES)))

ifeq ($(WINDOWS), 1)
  OBJECTS += obj/$(FILENAME)/windows_resources.o
endif

all: $(BUILD_DIRECTORY)/$(FILENAME) $(BUILD_DIRECTORY)/data
	$(info Finished)

$(BUILD_DIRECTORY)/data: $(DATA_DIRECTORY)
	@mkdir -p $(@D)
	@rm -rf $(BUILD_DIRECTORY)/data
	@cp -r $(DATA_DIRECTORY) $(BUILD_DIRECTORY)/data

$(BUILD_DIRECTORY)/$(FILENAME): $(OBJECTS)
	@mkdir -p $(@D)
	$(info Linking $@)
	@$(CXX) $(ALL_CXXFLAGS) $(CSE2_CXXFLAGS) $(ALL_LDFLAGS) $^ -o $@ $(ALL_LIBS) $(CSE2_LIBS)

obj/$(FILENAME)/%.o: %.c
	@mkdir -p $(@D)
	$(info Compiling $<)
	@$(CC) $(ALL_CFLAGS) $(CSE2_CFLAGS) $(DEFINES) $< -o $@ -c

obj/$(FILENAME)/%.o: %.cpp
	@mkdir -p $(@D)
	$(info Compiling $<)
	@$(CXX) $(ALL_CXXFLAGS) $(CSE2_CXXFLAGS) $(DEFINES) $< -o $@ -c

obj/$(FILENAME)/src/Resource.o: src/Resource.cpp $(addprefix src/Resource/, $(addsuffix .h, $(RESOURCES)))
	@mkdir -p $(@D)
	$(info Compiling $<)
	@$(CXX) $(ALL_CXXFLAGS) $(CSE2_CXXFLAGS) $(DEFINES) $< -o $@ -c

src/Resource/%.h: $(ASSETS_DIRECTORY)/resources/% obj/bin2h
	@mkdir -p $(@D)
	$(info Converting $<)
	@obj/bin2h $< $@

obj/bin2h: bin2h/bin2h.c
	@mkdir -p $(@D)
	$(info Compiling $^)
	@$(NATIVECC) -O3 -s -std=c90 -Wall -Wextra -pedantic $^ -o $@

include $(wildcard $(DEPENDENCIES))

obj/$(FILENAME)/windows_resources.o: $(ASSETS_DIRECTORY)/resources/CSE2.rc $(ASSETS_DIRECTORY)/resources/resource1.h $(ASSETS_DIRECTORY)/resources/afxres.h $(ASSETS_DIRECTORY)/resources/ICON/0.ico
	@mkdir -p $(@D)
	$(info Compiling Windows resource file $<)
	@$(WINDRES) $< $@

# TODO
clean:
	@rm -rf obj
