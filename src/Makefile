############################################################
# Tsunagari Tile Engine primary source Makefile (gnu make) #
############################################################

### --- VARIABLES SECTION --- ###

# Compiler to use. (Suggested: "g++" or "clang")
CXX = g++

# Warning flags to enable.
WFLAGS = -Wall -Wextra -Wconversion

# Internal Lua library.
LUACFLAGS = -I../deps/lua
LUALIB = ../deps/lua/liblua.a

# Libraries to include.
LIBS = $(LUALIB) -lgosu -lzip -lboost_program_options \
	$(shell xml2-config --libs)

# Compiler and linker flags.
CXXFLAGS = $(MYCFLAGS) -pedantic -ansi $(WFLAGS) \
	$(shell xml2-config --cflags) $(LUACFLAGS)
LDFLAGS = $(MYLDFLAGS) $(LIBS)

# Name of testing world.
TESTWORLD = babysfirst.world

### --- MAIN SECTION --- ###

all: tsunagari $(TESTWORLD)

depend:
	@$(CXX) $(CXXFLAGS) -MM *.cpp

tsunagari: $(LUALIB) animation.o area.o cmd.o common.o entity.o entity-lua.o \
           log.o main.o player.o resourcer.o script.o script-sound.o window.o \
           world.o
	$(CXX) -o tsunagari *.o $(LDFLAGS)

%.world: %/*
	cd $(basename $@) && zip -r -0 ../$@ *

clean:
	$(RM) tsunagari *.o $(TESTWORLD)

