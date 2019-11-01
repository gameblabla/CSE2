NATIVECC = cc
NATIVECXX = c++
WINDRES = windres
PKG_CONFIG = pkg-config

BUILD_DIRECTORY = game
ASSETS_DIRECTORY = assets

# Default options
RENDERER ?= SDLTexture

ifeq ($(WINDOWS), 1)
	EXE_EXTENSION = .exe
endif

ifeq ($(RELEASE), 1)
	CXXFLAGS = -O3
	LDFLAGS = -s
	FILENAME_DEF = CSE2$(EXE_EXTENSION)
	DOCONFIG_FILENAME_DEF = DoConfig$(EXE_EXTENSION)
else
	CXXFLAGS = -Og -ggdb3
	FILENAME_DEF = CSE2_debug$(EXE_EXTENSION)
	DOCONFIG_FILENAME_DEF = DoConfig_debug$(EXE_EXTENSION)
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

ifeq ($(DEBUG_SAVE), 1)
	CXXFLAGS += -DDEBUG_SAVE
endif

ifeq ($(WARNINGS), 1)
	CXXFLAGS += -Wall -Wextra -pedantic
endif

ifeq ($(ALL_WARNINGS), 1)
	ifneq ($(findstring clang,$(CXX)))
		# Use clang-specific flag -Weverything
		CXXFLAGS += -Weverything
	else
		@echo Couldn\'t activate all warnings (Unsupported compiler)
	endif
endif

ifeq ($(FATAL_WARNINGS), 1)
	CXXFLAGS += -Werror
endif

CXXFLAGS += -std=c++98 -MMD -MP -MF $@.d `$(PKG_CONFIG) sdl2 --cflags` `$(PKG_CONFIG) freetype2 --cflags`

ifeq ($(STATIC), 1)
	LDFLAGS += -static
	LIBS += `$(PKG_CONFIG) sdl2 --libs --static` `$(PKG_CONFIG) freetype2 --libs --static` -lfreetype
else
	LIBS += `$(PKG_CONFIG) sdl2 --libs` `$(PKG_CONFIG) freetype2 --libs`
endif

SOURCES = \
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
	src/Escape \
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
	src/NpChar \
	src/NpcHit \
	src/NpcTbl \
	src/Organya \
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
	ICON/ICON_MINI.bmp \
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

ifeq ($(RENDERER), OpenGL3)
	SOURCES += src/Backends/Rendering/OpenGL3
	CXXFLAGS += `$(PKG_CONFIG) glew --cflags`

	ifeq ($(STATIC), 1)
		CXXFLAGS += -DGLEW_STATIC
		LIBS += `$(PKG_CONFIG) glew --libs --static`
	else
		LIBS += `$(PKG_CONFIG) glew --libs`
	endif

	ifeq ($(WINDOWS), 1)
		LIBS += -lopengl32
	else
		LIBS += -lGL
	endif
else ifeq ($(RENDERER), SDLTexture)
	SOURCES += src/Backends/Rendering/SDLTexture
else ifeq ($(RENDERER), SDLSurface)
	SOURCES += src/Backends/Rendering/SDLSurface
else ifeq ($(RENDERER), Software)
	SOURCES += src/Backends/Rendering/Software
else
	@echo Invalid RENDERER selected; this build will fail
endif

OBJECTS = $(addprefix obj/$(FILENAME)/, $(addsuffix .o, $(SOURCES)))
DEPENDENCIES = $(addprefix obj/$(FILENAME)/, $(addsuffix .o.d, $(SOURCES)))

ifeq ($(WINDOWS), 1)
	OBJECTS += obj/$(FILENAME)/windows_resources.o
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

obj/$(FILENAME)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) $< -o $@ -c

obj/$(FILENAME)/src/Resource.o: src/Resource.cpp $(addprefix src/Resource/, $(addsuffix .h, $(RESOURCES)))
	@mkdir -p $(@D)
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) $< -o $@ -c

src/Resource/%.h: $(ASSETS_DIRECTORY)/resources/% obj/bin2h
	@mkdir -p $(@D)
	@echo Converting $<
	@obj/bin2h $< $@

obj/bin2h: bin2h/bin2h.c
	@mkdir -p $(@D)
	@echo Compiling $^
	@$(NATIVECC) -O3 -s -std=c90 -Wall -Wextra -pedantic $^ -o $@

include $(wildcard $(DEPENDENCIES))

obj/$(FILENAME)/windows_resources.o: $(ASSETS_DIRECTORY)/resources/CSE2.rc $(ASSETS_DIRECTORY)/resources/resource1.h $(ASSETS_DIRECTORY)/resources/afxres.h $(ASSETS_DIRECTORY)/resources/ICON/0.ico
	@mkdir -p $(@D)
	@echo Compiling Windows resource file $<
	@$(WINDRES) $< $@

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
