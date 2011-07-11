############################################################
# Tsunagari Tile Engine primary source Makefile (gnu make) #
############################################################

### --- VARIABLES SECTION --- ###

# Warning flags to enable.
WFLAGS = -Wall -Wextra -Wconversion

# Libraries to include.
LIBS = -lgosu -lzip -lboost_program_options `xml2-config --libs`

# Compiler and linker flags.
CXXFLAGS = -g -pedantic -ansi $(WFLAGS) `xml2-config --cflags`
LDFLAGS = -g $(LIBS)

# Compiler to use. (Suggested: "g++" or "clang")
CXX = g++

# Name of testing world.
TESTWORLD = babysfirst.world

### --- MAIN SECTION --- ###

all: tsunagari $(TESTWORLD)

tsunagari: area.o cmd.o common.o entity.o log.o main.o player.o resourcer.o window.o world.o
	$(CXX) -o tsunagari *.o $(LDFLAGS)

%.world: %/*
	cd $(basename $@) && zip -0 ../$@ *

.PHONY: doc
doc:
	cd ../doc && doxygen

clean:
	$(RM) tsunagari *.o $(TESTWORLD) && $(RM) -r ../doc/html

