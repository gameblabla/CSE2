RELEASE = 0

ifeq ($(RELEASE), 0)
CXXFLAGS := -O0 -g -static -mconsole
FILENAME = debug
else
CXXFLAGS := -O3 -s -static
FILENAME = release
endif

ifeq ($(JAPANESE), 1)
CXXFLAGS += -DJAPANESE
endif
ifeq ($(FIX_BUGS), 1)
CXXFLAGS += -DFIX_BUGS
endif

CXXFLAGS += `sdl2-config --cflags`
LIBS += `sdl2-config --static-libs` -lSDL2_ttf -lfreetype -lharfbuzz -lfreetype -lbz2 -lpng -lz -lgraphite2 -lRpcrt4 -lDwrite -lusp10

# For an accurate result to the original's code, compile in alphabetical order
SOURCES = \
	Back \
	Config \
	Draw \
	Ending \
	Escape \
	Fade \
	Flags \
	Game \
	Generic \
	GenericLoad \
	Input \
	KeyControl \
	Main \
	Map \
	MapName \
	NpChar \
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
	BITMAP/PIXEL.bmp \
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

OBJECTS = $(addprefix obj/$(FILENAME)/, $(addsuffix .o, $(SOURCES)))

all: build/$(FILENAME).exe

build/$(FILENAME).exe: $(OBJECTS)
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

src/Resource/%.h: res/% obj/bin2h.exe
	@mkdir -p $(@D)
	@echo Converting $<
	@obj/bin2h.exe $< $@

obj/bin2h.exe: res/bin2h.c
	@mkdir -p $(@D)
	@echo Compiling $^
	@gcc -O3 -s -static $^ -o $@

clean:
	@rm -rf build obj
