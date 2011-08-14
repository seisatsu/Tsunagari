############################################################
# Tsunagari Tile Engine primary source Makefile (gnu make) #
############################################################

### --- VARIABLES SECTION --- ###

# Warning flags to enable.
WFLAGS = -Wall -Wextra -Wconversion

# Internal Lua library.
LUACFLAGS = -I../deps/lua
LUALIB = ../deps/lua/liblua.a

# Libraries to include.
LIBS = $(LUALIB) -lgosu -lzip -lboost_program_options $(shell xml2-config --libs)

# Compiler and linker flags.
CXXFLAGS = -g -pedantic -ansi $(WFLAGS) $(shell xml2-config --cflags) $(LUACFLAGS)
LDFLAGS = -g $(LIBS)

# Compiler to use. (Suggested: "g++" or "clang")
CXX = g++

# Name of testing world.
TESTWORLD = babysfirst.world

### --- MAIN SECTION --- ###

all: tsunagari $(TESTWORLD)

tsunagari: $(LUALIB) animation.o area.o cmd.o common.o entity.o entity-lua.o log.o main.o player.o resourcer.o script.o script-sound.o window.o world.o
	$(CXX) -o tsunagari *.o $(LDFLAGS)

$(LUALIB):
	cd ../deps/lua && make linux

%.world: %/*
	cd $(basename $@) && zip -0 ../$@ *

.PHONY: doc
doc:
	cd ../doc && doxygen

clean:
	$(RM) tsunagari *.o $(TESTWORLD) && $(RM) -r ../doc/html

