FILENAME := CSE2.elf

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
			
OBJECTS = $(addprefix obj/$(FILENAME)/, $(addsuffix .o, $(SOURCES)))
DEPENDENCIES = $(addprefix obj/$(FILENAME)/, $(addsuffix .o.d, $(SOURCES)))

CXXFLAGS :=	$(MACHDEP) -I$(LIBOGC_INC)
LDFLAGS :=	-L$(LIBOGC_LIB) $(MACHDEP)
LIBS :=		-lwiiuse -lbte -lfat -logc -lm

ifeq ($(FIX_BUGS), 1)
	CXXFLAGS += -DFIX_BUGS
endif

all: build/$(FILENAME)

build/$(FILENAME): $(OBJECTS)
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)
	@echo Finished compiling: $@

obj/$(FILENAME)/%.o: src/%.cpp
	@mkdir -p $(@D)
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) $< -o $@ -c

include $(wildcard $(DEPENDENCIES))

clean:
	@rm -rf build obj
