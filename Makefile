WINDRES = windres

BUILD_DIRECTORY = game
ASSETS_DIRECTORY = assets

ifeq ($(RELEASE), 1)
	CXXFLAGS = -O3
	LDFLAGS = -s
	FILENAME_DEF = CSE2.exe
else
	CXXFLAGS = -Og -ggdb3
	FILENAME_DEF = CSE2_debug.exe
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

ifeq ($(CONSOLE), 1)
	CXXFLAGS += -mconsole
else
	CXXFLAGS += -mwindows
endif

ifeq ($(DEBUG_SAVE), 1)
	CXXFLAGS += -DDEBUG_SAVE
endif

CXXFLAGS += -std=c++98 -MMD -MP -MF $@.d
LIBS += -lkernel32 -lgdi32 -lddraw -ldinput -ldsound -lversion -lshlwapi -limm32 -lwinmm -ldxguid

ifeq ($(STATIC), 1)
	LDFLAGS += -static
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
	src/Dialog \
	src/Draw \
	src/Ending \
	src/Escape \
	src/Fade \
	src/Flags \
	src/Flash \
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
	src/SelStage \
	src/Shoot \
	src/Sound \
	src/Stage \
	src/Star \
	src/TextScr \
	src/Triangle \
	src/ValueView

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
	BITMAP/pixel.bmp \
	CURSOR/CURSOR_IKA.cur \
	CURSOR/CURSOR_NORMAL.cur \
	ICON/0.ico \
	ICON/ICON_MINI.ico \
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

OBJECTS = $(addprefix obj/$(FILENAME)/, $(addsuffix .o, $(SOURCES)))
DEPENDENCIES = $(addprefix obj/$(FILENAME)/, $(addsuffix .o.d, $(SOURCES)))

OBJECTS += obj/$(FILENAME)/windows_resources.o

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

obj/$(FILENAME)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) $< -o $@ -c

include $(wildcard $(DEPENDENCIES))

obj/$(FILENAME)/windows_resources.o: $(ASSETS_DIRECTORY)/resources/CSE2.rc $(ASSETS_DIRECTORY)/resources/resource1.h $(ASSETS_DIRECTORY)/resources/afxres.h $(addprefix $(ASSETS_DIRECTORY)/resources/, $(RESOURCES))
	@mkdir -p $(@D)
	@echo Compiling Windows resource file $<
	@$(WINDRES) $< $@

# TODO
clean:
	@rm -rf obj
