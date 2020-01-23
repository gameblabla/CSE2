NATIVECC = cc
NATIVECXX = c++
WINDRES = windres
PKGCONFIG = pkg-config

BUILD_DIRECTORY = game
ASSETS_DIRECTORY = assets

# Default options
RENDERER = SDLTexture

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
  DOCONFIG_FILENAME_DEF = DoConfig$(EXE_EXTENSION)
else
  ALL_CFLAGS += -Og -ggdb3
  ALL_CXXFLAGS += -Og -ggdb3
  FILENAME_DEF = CSE2_debug$(EXE_EXTENSION)
  DOCONFIG_FILENAME_DEF = DoConfig_debug$(EXE_EXTENSION)
endif

ifeq ($(LTO), 1)
  ALL_CFLAGS += -flto
  ALL_CXXFLAGS += -flto
endif

ifeq ($(JAPANESE), 1)
  DATA_DIRECTORY = $(ASSETS_DIRECTORY)/data_jp

  DEFINES += -DJAPANESE
else
  DATA_DIRECTORY = $(ASSETS_DIRECTORY)/data_en
endif

FILENAME ?= $(FILENAME_DEF)
DOCONFIG_FILENAME ?= $(DOCONFIG_FILENAME_DEF)

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
    # Use Clang-specific flag -Weverything
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
CSE2_CFLAGS += $(shell $(PKGCONFIG) sdl2 --cflags) $(shell $(PKGCONFIG) freetype2 --cflags)

ALL_CXXFLAGS += -std=c++98 -MMD -MP -MF $@.d
CSE2_CXXFLAGS += $(shell $(PKGCONFIG) sdl2 --cflags) $(shell $(PKGCONFIG) freetype2 --cflags)

ifeq ($(STATIC), 1)
  ALL_LDFLAGS += -static
  CSE2_LIBS += $(shell $(PKGCONFIG) sdl2 --libs --static) $(shell $(PKGCONFIG) freetype2 --libs --static) -lfreetype
  DOCONFIG_LIBS += $(shell fltk-config --cxxflags --libs --ldstaticflags)
else
  CSE2_LIBS += $(shell $(PKGCONFIG) sdl2 --libs) $(shell $(PKGCONFIG) freetype2 --libs)
  DOCONFIG_LIBS += $(shell fltk-config --cxxflags --libs --ldflags)
endif

SOURCES = \
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
  src/NpChar.cpp \
  src/NpcHit.cpp \
  src/NpcTbl.cpp \
  src/Organya.cpp \
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

RESOURCES = \
  BITMAP/Credit01.bmp \
  BITMAP/Credit02.bmp \
  BITMAP/Credit03.bmp \
  BITMAP/Credit04.bmp \
  BITMAP/Credit05.bmp \
  BITMAP/Credit06.bmp \
  BITMAP/Credit07.bmp \
  BITMAP/Credit08.bmp \
  BITMAP/Credit09.bmp \
  BITMAP/Credit10.bmp \
  BITMAP/Credit11.bmp \
  BITMAP/Credit12.bmp \
  BITMAP/Credit14.bmp \
  BITMAP/Credit15.bmp \
  BITMAP/Credit16.bmp \
  BITMAP/Credit17.bmp \
  BITMAP/Credit18.bmp \
  CURSOR/CURSOR_IKA.bmp \
  CURSOR/CURSOR_NORMAL.bmp \
  ORG/Access.org \
  ORG/Anzen.org \
  ORG/Balcony.org \
  ORG/Ballos.org \
  ORG/BreakDown.org \
  ORG/Cemetery.org \
  ORG/Curly.org \
  ORG/Dr.org \
  ORG/Ending.org \
  ORG/Escape.org \
  ORG/Fanfale1.org \
  ORG/Fanfale2.org \
  ORG/Fanfale3.org \
  ORG/FireEye.org \
  ORG/Gameover.org \
  ORG/Ginsuke.org \
  ORG/Grand.org \
  ORG/Gravity.org \
  ORG/Hell.org \
  ORG/ironH.org \
  ORG/Jenka.org \
  ORG/Jenka2.org \
  ORG/Kodou.org \
  ORG/LastBtl3.org \
  ORG/LastBtl.org \
  ORG/LastCave.org \
  ORG/Marine.org \
  ORG/Maze.org \
  ORG/MDown2.org \
  ORG/Mura.org \
  ORG/Oside.org \
  ORG/Plant.org \
  ORG/quiet.org \
  ORG/Requiem.org \
  ORG/Toroko.org \
  ORG/Vivi.org \
  ORG/Wanpak2.org \
  ORG/Wanpaku.org \
  ORG/Weed.org \
  ORG/White.org \
  ORG/XXXX.org \
  ORG/Zonbie.org \
  WAVE/Wave.dat

ifeq ($(JAPANESE), 1)
  RESOURCES += BITMAP/pixel_jp.bmp
  RESOURCES += FONT/NotoSansMonoCJKjp.otf
else
  RESOURCES += BITMAP/pixel.bmp
  RESOURCES += FONT/LiberationMono.ttf
endif

ifneq ($(WINDOWS), 1)
  RESOURCES += ICON/ICON_MINI.bmp
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
else ifeq ($(RENDERER), OpenGLES2)
  SOURCES += src/Backends/Rendering/OpenGLES2.cpp
  CSE2_CFLAGS += $(shell $(PKGCONFIG) --cflags glesv2)
  CSE2_CXXFLAGS += $(shell $(PKGCONFIG) --cflags glesv2)
  CSE2_LIBS += $(shell $(PKGCONFIG) --libs glesv2)
else ifeq ($(RENDERER), SDLTexture)
  SOURCES += src/Backends/Rendering/SDLTexture.cpp
else ifeq ($(RENDERER), SDLSurface)
  SOURCES += src/Backends/Rendering/SDLSurface.cpp
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

all: $(BUILD_DIRECTORY)/$(FILENAME) $(BUILD_DIRECTORY)/data $(BUILD_DIRECTORY)/$(DOCONFIG_FILENAME)
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

obj/$(FILENAME)/src/Resource.cpp.o: src/Resource.cpp $(addprefix src/Resource/, $(addsuffix .h, $(RESOURCES)))
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

$(BUILD_DIRECTORY)/$(DOCONFIG_FILENAME): DoConfig/DoConfig.cpp
	@mkdir -p $(@D)
	$(info Linking $@)
	@$(CXX) $(ALL_CXXFLAGS) $(ALL_LDFLAGS) $^ -o $@ $(DOCONFIG_LIBS)

# TODO
clean:
	@rm -rf obj
