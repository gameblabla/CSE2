PRGNAME     = cave
CC			= gcc
CXX			= g++
STRIP		= strip

SRCDIR		= ./src ./src/Backends/Controller ./src/Backends/Platform ./src/Backends/Audio/SoftwareMixer ./src/Backends/Audio ./src/Backends/Rendering ./src/Backends/Rendering/Window/Software
VPATH		= $(SRCDIR)
SRC_C		= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
SRC_CP		= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.cpp))
OBJ_C		= $(notdir $(patsubst %.c, %.o, $(SRC_C)))
OBJ_CP		= $(notdir $(patsubst %.cpp, %.o, $(SRC_CP)))
OBJS		= $(OBJ_C) $(OBJ_CP)

CFLAGS		= -O0 -g3
CFLAGS		+= -DALIGN_DWORD -Wall -Wextra
CFLAGS		+= -DLSB_FIRST -Isrc/Backends/Shared -Iassets -I/usr/include/SDL
CFLAGS		+= -I./src -I./src/Backends/Controller -I./src/Backends/Platform -I./src/Backends/Audio/SoftwareMixer -I./src/Backends/Audio -I./src/Backends/Rendering -I./src/Backends/Rendering/Window/Software


ifeq ($(PROFILE), YES)
CFLAGS 		+= -fprofile-generate=./
else ifeq ($(PROFILE), APPLY)
CFLAGS		+= -fprofile-use
endif

CXXFLAGS 	= $(CFLAGS) -std=gnu++14

LDFLAGS     = -lc -lgcc -lstdc++ -lrt -ldl -lm -lSDL -lasound -lpthread -ldrm -ludev

ifeq ($(PROFILE), YES)
LDFLAGS 	+= -lgcov
endif

# Rules to make executable
$(PRGNAME): $(OBJS)  
	$(CC) $(CFLAGS) -o $(PRGNAME) $^ $(LDFLAGS)

$(OBJ_C) : %.o : %.c
	$(CC) $(CFLAGS) -std=gnu99 -c -o $@ $<

$(OBJ_CP) : %.o : %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(PRGNAME) *.o
