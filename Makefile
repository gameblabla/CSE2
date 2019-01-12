RELEASE = 0

ifeq ($(RELEASE), 0)
CXXFLAGS := -O0 -g -static
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
LIBS += `sdl2-config --static-libs`

# For an accurate result to the original's code, compile in alphabetical order
SOURCES = \
	Config \
	Input \
	KeyControl \
	Main \

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

clean:
	@rm -rf build obj
