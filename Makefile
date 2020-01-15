NATIVECC = cc
NATIVECXX = c++
WINDRES = windres
PKGCONFIG = pkg-config

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
CSE2_CFLAGS += $(shell $(PKGCONFIG) sdl2 --cflags) $(shell $(PKGCONFIG) freetype2 --cflags) -Iexternal

ALL_CXXFLAGS += -std=c++11 -MMD -MP -MF $@.d
CSE2_CXXFLAGS += $(shell $(PKGCONFIG) sdl2 --cflags) $(shell $(PKGCONFIG) freetype2 --cflags) -Iexternal

DEFINES += -DLODEPNG_NO_COMPILE_ENCODER -DLODEPNG_NO_COMPILE_ERROR_TEXT -DLODEPNG_NO_COMPILE_CPP

ifeq ($(STATIC), 1)
  ALL_LDFLAGS += -static
  CSE2_LIBS += $(shell $(PKGCONFIG) sdl2 --libs --static) $(shell $(PKGCONFIG) freetype2 --libs --static) -lfreetype
else
  CSE2_LIBS += $(shell $(PKGCONFIG) sdl2 --libs) $(shell $(PKGCONFIG) freetype2 --libs)
endif

SOURCES = \
  external/lodepng/lodepng.cpp \
  src/ArmsItem.cpp \
  src/Back.cpp \
  src/Boss.cpp \
  src/BossAlmo1.cpp \
  src/BossAlmo2.cpp \
  src/BossBallos.cpp \
  src/BossFrog.cpp \
  src/BossIronH.cpp \
  src/BossLife.cpp \
  src/BossOhm.cpp \
  src/BossPress.cpp \
  src/BossTwinD.cpp \
  src/BossX.cpp \
  src/BulHit.cpp \
  src/Bullet.cpp \
  src/Caret.cpp \
  src/Config.cpp \
  src/Draw.cpp \
  src/Ending.cpp \
  src/Escape.cpp \
  src/Fade.cpp \
  src/File.cpp \
  src/Flags.cpp \
  src/Flash.cpp \
  src/Font.cpp \
  src/Frame.cpp \
  src/Game.cpp \
  src/Generic.cpp \
  src/GenericLoad.cpp \
  src/Input.cpp \
  src/KeyControl.cpp \
  src/Main.cpp \
  src/Map.cpp \
  src/MapName.cpp \
  src/MiniMap.cpp \
  src/MyChar.cpp \
  src/MycHit.cpp \
  src/MycParam.cpp \
  src/NpcAct000.cpp \
  src/NpcAct020.cpp \
  src/NpcAct040.cpp \
  src/NpcAct060.cpp \
  src/NpcAct080.cpp \
  src/NpcAct100.cpp \
  src/NpcAct120.cpp \
  src/NpcAct140.cpp \
  src/NpcAct160.cpp \
  src/NpcAct180.cpp \
  src/NpcAct200.cpp \
  src/NpcAct220.cpp \
  src/NpcAct240.cpp \
  src/NpcAct260.cpp \
  src/NpcAct280.cpp \
  src/NpcAct300.cpp \
  src/NpcAct320.cpp \
  src/NpcAct340.cpp \
  src/NpcAct360.cpp \
  src/NpcAct380.cpp \
  src/NpChar.cpp \
  src/NpcHit.cpp \
  src/NpcTbl.cpp \
  src/Organya.cpp \
  src/Pause.cpp \
  src/PixTone.cpp \
  src/Profile.cpp \
  src/Random.cpp \
  src/Resource.cpp \
  src/SelStage.cpp \
  src/Shoot.cpp \
  src/Sound.cpp \
  src/Stage.cpp \
  src/Star.cpp \
  src/TextScr.cpp \
  src/Triangle.cpp \
  src/ValueView.cpp \
  src/Backends/Audio/SDL2.cpp

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

  ALL_CFLAGS += $(shell $(PKGCONFIG) libxmp-lite --cflags)
  ALL_CXXFLAGS += $(shell $(PKGCONFIG) libxmp-lite --cflags)

  ifeq ($(STATIC), 1)
    ALL_LIBS += $(shell $(PKGCONFIG) libxmp-lite --libs --static)
  else
    ALL_LIBS += $(shell $(PKGCONFIG) libxmp-lite --libs)
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
  SOURCES += src/Backends/Rendering/OpenGL3.cpp external/glad/src/glad.c
  CSE2_CFLAGS += -Iexternal/glad/include
  CSE2_CXXFLAGS += -Iexternal/glad/include

  ifeq ($(WINDOWS), 1)
    CSE2_LIBS += -lopengl32
  else
    CSE2_LIBS += -lGL -ldl
  endif
else ifeq ($(RENDERER), SDLTexture)
  SOURCES += src/Backends/Rendering/SDLTexture.cpp
else ifeq ($(RENDERER), Software)
  SOURCES += src/Backends/Rendering/Software.cpp
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

obj/$(FILENAME)/%.c.o: %.c
	@mkdir -p $(@D)
	$(info Compiling $<)
	@$(CC) $(ALL_CFLAGS) $(CSE2_CFLAGS) $(DEFINES) $< -o $@ -c

obj/$(FILENAME)/%.cpp.o: %.cpp
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
