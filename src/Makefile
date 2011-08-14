############################################################
# Tsunagari Tile Engine primary source Makefile (gnu make) #
############################################################

### --- VARIABLES SECTION --- ###

# Warning flags to enable.
WFLAGS = -Wall -Wextra -Wconversion

# Libraries to include.
LIBS = -lgosu -lzip -lboost_program_options `xml2-config --libs` `pkg-config --libs lua5.1`

# Compiler and linker flags.
CXXFLAGS = -g -pedantic -ansi $(WFLAGS) `xml2-config --cflags` `pkg-config --cflags lua5.1`
LDFLAGS = -g $(LIBS)

# Compiler to use. (Suggested: "g++" or "clang")
CXX = g++

# Name of testing world.
TESTWORLD = babysfirst.world

### --- MAIN SECTION --- ###

all: tsunagari $(TESTWORLD)

tsunagari: animation.o area.o cmd.o common.o entity.o entity-lua.o log.o main.o player.o resourcer.o script.o script-sound.o window.o world.o
	$(CXX) -o tsunagari *.o $(LDFLAGS)

%.world: %/*
	cd $(basename $@) && zip -0 ../$@ *

.PHONY: doc
doc:
	cd ../doc && doxygen

clean:
	$(RM) tsunagari *.o $(TESTWORLD) && $(RM) -r ../doc/html

