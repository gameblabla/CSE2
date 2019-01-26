ifeq ($(RELEASE), 1)
CXXFLAGS := -O3 -s
FILENAME := release
else
CXXFLAGS := -O0 -g -mconsole
FILENAME := debug
endif

ifeq ($(JAPANESE), 1)
CXXFLAGS += -DJAPANESE
LIBS += -liconv
	ifeq ($(RELEASE), 1)
	FILENAME := releasejp
	else
	FILENAME := debugjp
	endif
endif

ifeq ($(FIX_BUGS), 1)
CXXFLAGS += -DFIX_BUGS
endif

CXXFLAGS += `sdl2-config --cflags` `pkg-config freetype2 --cflags`
LIBS += `sdl2-config --static-libs` `pkg-config freetype2 --libs`

ifeq ($(STATIC), 1)
CXXFLAGS += -static
LIBS += -lharfbuzz -lfreetype -lbz2 -lpng -lz -lgraphite2 -lRpcrt4 -lDwrite -lusp10
endif

# For an accurate result to the original's code, compile in alphabetical order
SOURCES = \
	Back \
	Caret \
	Config \
	Draw \
	Ending \
	Escape \
	Fade \
	Flags \
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
	MyChar \
	MycHit \
	NpcAct000 \
	NpChar \
	NpcHit \
	NpcTbl \
	Organya \
	PixTone \
	Profile \
	Resource \
	Sound \
	Stage \
	TextScr \
	Triangle

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
	ICON/4.bmp \
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
	RESOURCES += BITMAP/PIXEL_JP.bmp
else
	RESOURCES += BITMAP/PIXEL.bmp
endif

OBJECTS = $(addprefix obj/$(FILENAME)/, $(addsuffix .o, $(SOURCES)))

all: build/$(FILENAME)

build/$(FILENAME): $(OBJECTS)
	@mkdir -p $(@D)
	@g++ $(CXXFLAGS) $^ -o $@ $(LIBS)
	@echo Finished compiling: $@

obj/$(FILENAME)/%.o: src/%.cpp
	@mkdir -p $(@D)
	@echo Compiling $^
	@g++ $(CXXFLAGS) $^ -o $@ -c

obj/$(FILENAME)/Resource.o: src/Resource.cpp $(addprefix src/Resource/, $(addsuffix .h, $(RESOURCES)))
	@mkdir -p $(@D)
	@echo Compiling $<
	@g++ $(CXXFLAGS) $< -o $@ -c

src/Resource/%.h: res/% obj/bin2h
	@mkdir -p $(@D)
	@echo Converting $<
	@obj/bin2h $< $@

obj/bin2h: res/bin2h.c
	@mkdir -p $(@D)
	@echo Compiling $^
	@gcc -O3 -s -static $^ -o $@

clean:
	@rm -rf build obj
