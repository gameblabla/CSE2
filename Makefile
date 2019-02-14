ifeq ($(RELEASE), 1)
	CXXFLAGS = -O3 -s
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

CXXFLAGS += `sdl2-config --cflags` `pkg-config freetype2 --cflags` -MMD -MP -MF $@.d -DLODEPNG_NO_COMPILE_ENCODER -DLODEPNG_NO_COMPILE_DISK -DLODEPNG_NO_COMPILE_ERROR_TEXT -DLODEPNG_NO_COMPILE_CPP
LIBS += `sdl2-config --static-libs` `pkg-config freetype2 --libs`

ifeq ($(STATIC), 1)
	CXXFLAGS += -static
	LIBS += -lharfbuzz -lfreetype -lbz2 -lpng -lz -lgraphite2 -lRpcrt4 -lDwrite -lusp10
endif

# For an accurate result to the original's code, compile in alphabetical order
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
	Shoot \
	Sound \
	Stage \
	Star \
	SelStage \
	TextScr \
	Triangle \
	ValueView

RESOURCES = \
	BITMAP/CREDIT01.png \
	BITMAP/CREDIT02.png \
	BITMAP/CREDIT03.png \
	BITMAP/CREDIT04.png \
	BITMAP/CREDIT05.png \
	BITMAP/CREDIT06.png \
	BITMAP/CREDIT07.png \
	BITMAP/CREDIT08.png \
	BITMAP/CREDIT09.png \
	BITMAP/CREDIT10.png \
	BITMAP/CREDIT11.png \
	BITMAP/CREDIT12.png \
	BITMAP/CREDIT14.png \
	BITMAP/CREDIT15.png \
	BITMAP/CREDIT16.png \
	BITMAP/CREDIT17.png \
	BITMAP/CREDIT18.png \
	CURSOR/CURSOR_IKA.bmp \
	CURSOR/CURSOR_NORMAL.bmp \
	ORG/ACCESS \
	ORG/ANZEN \
	ORG/BALCONY \
	ORG/BALLOS \
	ORG/BDOWN \
	ORG/CEMETERY \
	ORG/CURLY \
	ORG/DR \
	ORG/ENDING \
	ORG/ESCAPE \
	ORG/FANFALE1 \
	ORG/FANFALE2 \
	ORG/FANFALE3 \
	ORG/FIREEYE \
	ORG/GAMEOVER \
	ORG/GINSUKE \
	ORG/GRAND \
	ORG/GRAVITY \
	ORG/HELL \
	ORG/IRONH \
	ORG/JENKA \
	ORG/JENKA2 \
	ORG/KODOU \
	ORG/LASTBT3 \
	ORG/LASTBTL \
	ORG/LASTCAVE \
	ORG/MARINE \
	ORG/MAZE \
	ORG/MDOWN2 \
	ORG/MURA \
	ORG/OSIDE \
	ORG/PLANT \
	ORG/QUIET \
	ORG/REQUIEM \
	ORG/TOROKO \
	ORG/VIVI \
	ORG/WANPAK2 \
	ORG/WANPAKU \
	ORG/WEED \
	ORG/WHITE \
	ORG/XXXX \
	ORG/ZONBIE \
	WAVE/WAVE100

ifeq ($(JAPANESE), 1)
	RESOURCES += BITMAP/PIXEL_JP.png
else
	RESOURCES += BITMAP/PIXEL.png
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
	@$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)
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
