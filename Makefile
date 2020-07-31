NATIVECC = cc
NATIVECXX = c++

BUILD_DIRECTORY = game
ASSETS_DIRECTORY = assets

ifeq ($(RELEASE), 1)
	CXXFLAGS = -O3 -flto
	LDFLAGS = -s
	FILENAME_DEF = CSE2
else
	CXXFLAGS = -Og -g3
	FILENAME_DEF = CSE2_debug
endif

ifeq ($(JAPANESE), 1)
	DATA_DIRECTORY = $(ASSETS_DIRECTORY)/data_jp

	CXXFLAGS += -DJAPANESE
else
	DATA_DIRECTORY = $(ASSETS_DIRECTORY)/data_en
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

CXXFLAGS += -std=c++98 `pkg-config sdl2 --cflags` `pkg-config freetype2 --cflags` -MMD -MP -MF $@.d

ifeq ($(STATIC), 1)
	LDFLAGS += -static
	LIBS += `pkg-config sdl2 --libs --static` `pkg-config freetype2 --libs --static` -lfreetype
else
	LIBS += `pkg-config sdl2 --libs` `pkg-config freetype2 --libs`
endif

# For an accurate result to the original's code, compile in alphabetical order
SOURCES = \
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
	MainLoop \
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
	SoftwareMixer \
	Sound \
	Stage \
	Star \
	TextScr \
	Triangle \
	ValueView

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
	RESOURCES += FONT/msgothic.ttc
else
	RESOURCES += BITMAP/pixel.bmp

	ifneq ($(WINDOWS), 1)
		RESOURCES += FONT/cour.ttf
	endif
endif

ifneq ($(WINDOWS), 1)
	RESOURCES += ICON/ICON_MINI.bmp
endif

OBJECTS = $(addprefix obj/$(FILENAME)/, $(addsuffix .o, $(SOURCES)))
DEPENDENCIES = $(addprefix obj/$(FILENAME)/, $(addsuffix .o.d, $(SOURCES)))

ifeq ($(WINDOWS), 1)
	OBJECTS += obj/$(FILENAME)/win_icon.o
endif

all: $(BUILD_DIRECTORY)/$(FILENAME) $(BUILD_DIRECTORY)/data
	@echo Finished

$(BUILD_DIRECTORY)/data: $(DATA_DIRECTORY)
	@mkdir -p $(@D)
	@rm -rf $(BUILD_DIRECTORY)/data
	@cp -r $(DATA_DIRECTORY) $(BUILD_DIRECTORY)/data

$(BUILD_DIRECTORY)/$(FILENAME): $(OBJECTS)
	@mkdir -p $(@D)
	@echo Linking $@
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

obj/$(FILENAME)/%.o: src/%.cpp
	@mkdir -p $(@D)
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) $< -o $@ -c

obj/$(FILENAME)/Resource.o: src/Resource.cpp $(addprefix src/Resource/, $(addsuffix .h, $(RESOURCES)))
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

obj/$(FILENAME)/win_icon.o: $(ASSETS_DIRECTORY)/resources/ICON/ICON.rc $(ASSETS_DIRECTORY)/resources/ICON/0.ico $(ASSETS_DIRECTORY)/resources/ICON/ICON_MINI.ico
	@mkdir -p $(@D)
	@windres $< $@

# TODO
clean:
	@rm -rf obj
