ifeq ($(RELEASE), 1)
	CXXFLAGS = -O3 -flto
	LDFLAGS = -s
	FILENAME_DEF = release
else
	CXXFLAGS = -O0 -g
	FILENAME_DEF = debug
endif

ifeq ($(JAPANESE), 1)
	CXXFLAGS += -DJAPANESE

	ifeq ($(RELEASE), 1)
		FILENAME_DEF = releasejp
	else
		FILENAME_DEF = debugjp
	endif
endif

FILENAME ?= $(FILENAME_DEF)

ifeq ($(FIX_BUGS), 1)
	CXXFLAGS += -DFIX_BUGS
endif

ifeq ($(WINDOWS), 1)
	ifeq ($(CONSOLE), 1)
		CXXFLAGS += -mconsole
	endif
	ifeq ($(JAPANESE), 1)
		LIBS += -liconv
	endif

	CXXFLAGS += -DWINDOWS
	LIBS += -lkernel32
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

RESOURCES = \
	BITMAP/CREDIT01.bmp \
	BITMAP/CREDIT02.bmp \
	BITMAP/CREDIT03.bmp \
	BITMAP/CREDIT04.bmp \
	BITMAP/CREDIT05.bmp \
	BITMAP/CREDIT06.bmp \
	BITMAP/CREDIT07.bmp \
	BITMAP/CREDIT08.bmp \
	BITMAP/CREDIT09.bmp \
	BITMAP/CREDIT10.bmp \
	BITMAP/CREDIT11.bmp \
	BITMAP/CREDIT12.bmp \
	BITMAP/CREDIT14.bmp \
	BITMAP/CREDIT15.bmp \
	BITMAP/CREDIT16.bmp \
	BITMAP/CREDIT17.bmp \
	BITMAP/CREDIT18.bmp \
	CURSOR/CURSOR_IKA.bmp \
	CURSOR/CURSOR_NORMAL.bmp \
	ORG/ACCESS.org \
	ORG/ANZEN.org \
	ORG/BALCONY.org \
	ORG/BALLOS.org \
	ORG/BDOWN.org \
	ORG/CEMETERY.org \
	ORG/CURLY.org \
	ORG/DR.org \
	ORG/ENDING.org \
	ORG/ESCAPE.org \
	ORG/FANFALE1.org \
	ORG/FANFALE2.org \
	ORG/FANFALE3.org \
	ORG/FIREEYE.org \
	ORG/GAMEOVER.org \
	ORG/GINSUKE.org \
	ORG/GRAND.org \
	ORG/GRAVITY.org \
	ORG/HELL.org \
	ORG/IRONH.org \
	ORG/JENKA.org \
	ORG/JENKA2.org \
	ORG/KODOU.org \
	ORG/LASTBT3.org \
	ORG/LASTBTL.org \
	ORG/LASTCAVE.org \
	ORG/MARINE.org \
	ORG/MAZE.org \
	ORG/MDOWN2.org \
	ORG/MURA.org \
	ORG/OSIDE.org \
	ORG/PLANT.org \
	ORG/QUIET.org \
	ORG/REQUIEM.org \
	ORG/TOROKO.org \
	ORG/VIVI.org \
	ORG/WANPAK2.org \
	ORG/WANPAKU.org \
	ORG/WEED.org \
	ORG/WHITE.org \
	ORG/XXXX.org \
	ORG/ZONBIE.org \
	WAVE/WAVE100

ifeq ($(JAPANESE), 1)
	RESOURCES += BITMAP/PIXEL_JP.bmp
else
	RESOURCES += BITMAP/PIXEL.bmp
endif

ifneq ($(WINDOWS), 1)
	RESOURCES += ICON/ICON_MINI.bmp
endif

OBJECTS = $(addprefix obj/$(FILENAME)/, $(addsuffix .o, $(SOURCES)))
DEPENDENCIES = $(addprefix obj/$(FILENAME)/, $(addsuffix .o.d, $(SOURCES)))

ifeq ($(WINDOWS), 1)
	OBJECTS += obj/$(FILENAME)/win_icon.o
endif

all: build/$(FILENAME)

build/$(FILENAME): $(OBJECTS)
	@mkdir -p $(@D)
	@echo Linking
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)
	@echo Finished compiling: $@

obj/$(FILENAME)/%.o: src/%.cpp
	@mkdir -p $(@D)
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) $< -o $@ -c

obj/$(FILENAME)/Resource.o: src/Resource.cpp $(addprefix src/Resource/, $(addsuffix .h, $(RESOURCES)))
	@mkdir -p $(@D)
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) $< -o $@ -c

src/Resource/%.h: res/% obj/bin2h
	@mkdir -p $(@D)
	@echo Converting $<
	@obj/bin2h $< $@

obj/bin2h: res/bin2h.c
	@mkdir -p $(@D)
	@echo Compiling $^
	@$(CC) -O3 -s -static $^ -o $@

include $(wildcard $(DEPENDENCIES))

obj/$(FILENAME)/win_icon.o: res/ICON/ICON.rc res/ICON/0.ico res/ICON/ICON_MINI.ico
	@mkdir -p $(@D)
	@windres $< $@

clean:
	@rm -rf build obj
