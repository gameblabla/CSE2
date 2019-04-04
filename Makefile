FILENAME := CSE2Wii

include $(DEVKITPPC)/wii_rules

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
			ValueView \
			EasyBMP/EasyBMP \
			
OBJECTS = $(addprefix obj/$(FILENAME)/, $(addsuffix .o, $(SOURCES)))
DEPENDENCIES = $(addprefix obj/$(FILENAME)/, $(addsuffix .o.d, $(SOURCES)))

CXXFLAGS :=	$(MACHDEP) -I$(LIBOGC_INC) -O3 -s
LDFLAGS :=	-L$(LIBOGC_LIB) $(MACHDEP)
LIBS :=		-lwiiuse -lbte -lfat -lm -logc

ifeq ($(FIX_BUGS), 1)
	CXXFLAGS += -DFIX_BUGS
endif

ifeq ($(JAPANESE), 1)
	CXXFLAGS += -DJAPANESE
endif

all: build/$(FILENAME).elf

build/$(FILENAME).elf: $(OBJECTS)
	@mkdir -p $(@D)
	@echo Linking
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)
	@echo Finished compiling: $@
	elf2dol build/$(FILENAME).elf build/boot.dol

obj/$(FILENAME)/%.o: src/%.cpp
	@mkdir -p $(@D)
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) $< -o $@ -c

include $(wildcard $(DEPENDENCIES))

clean:
	@rm -rf build obj
