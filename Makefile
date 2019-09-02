NATIVECC = cc
NATIVECXX = c++
WINDRES = windres

BUILD_DIRECTORY = game
ASSETS_DIRECTORY = assets

# Default options
RENDERER ?= Texture

ifeq ($(RELEASE), 1)
	CXXFLAGS = -O3
	LDFLAGS = -s
	FILENAME_DEF = CSE2
	DOCONFIG_FILENAME_DEF = DoConfig
else
	CXXFLAGS = -Og -ggdb3
	FILENAME_DEF = CSE2_debug
	DOCONFIG_FILENAME_DEF = DoConfig_debug
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

ifeq ($(CONSOLE), 1)
	CXXFLAGS += -mconsole
else
	CXXFLAGS += -mwindows
endif

ifeq ($(DEBUG_SAVE), 1)
	CXXFLAGS += -DDEBUG_SAVE
endif

CXXFLAGS += -std=c++98 -MMD -MP -MF $@.d -DWINDOWS -DNONPORTABLE `pkg-config sdl2 --cflags`
LIBS += -lkernel32 -lgdi32 -lddraw -ldinput -ldsound -lversion -lshlwapi -limm32 -lwinmm -ldxguid

ifeq ($(STATIC), 1)
	LDFLAGS += -static
	LIBS += `pkg-config sdl2 --libs --static`
else
	LIBS += `pkg-config sdl2 --libs`
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

ifeq ($(RENDERER), OpenGL3)
	SOURCES += src/Backends/Rendering/OpenGL3
	CXXFLAGS += `pkg-config glew --cflags`

	ifeq ($(STATIC), 1)
		CXXFLAGS += -DGLEW_STATIC
		LIBS += `pkg-config glew --libs --static`
	else
		LIBS += `pkg-config glew --libs`
	endif

#	ifeq ($(WINDOWS), 1)
		LIBS += -lopengl32
#	else
#		LIBS += -lGL
#	endif
else ifeq ($(RENDERER), Texture)
	SOURCES += src/Backends/Rendering/SDLTexture
else ifeq ($(RENDERER), Surface)
	SOURCES += src/Backends/Rendering/SDLSurface
else ifeq ($(RENDERER), Software)
	SOURCES += src/Backends/Rendering/Software
else
	@echo Invalid RENDERER selected; this build will fail
endif

OBJECTS = $(addprefix obj/$(FILENAME)/, $(addsuffix .o, $(SOURCES)))
DEPENDENCIES = $(addprefix obj/$(FILENAME)/, $(addsuffix .o.d, $(SOURCES)))

OBJECTS += obj/$(FILENAME)/windows_resources.o

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

include $(wildcard $(DEPENDENCIES))

obj/$(FILENAME)/windows_resources.o: msvc2003/CSE2.rc
	@mkdir -p $(@D)
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
