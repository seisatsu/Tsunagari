############################################################
# Tsunagari Tile Engine primary source Makefile (gnu make) #
############################################################

### --- VARIABLES SECTION --- ###

# Warning flags to enable.
WFLAGS = -Wall -Wextra -Wconversion

# Compiler and linker flags.
CXXFLAGS = -g -pedantic -ansi $(WFLAGS) `xml2-config --cflags`
LDFLAGS = -g -lgosu -lzip `xml2-config --libs`

# Compiler to use. (Suggested: "g++" or "clang")
CXX = g++

# Name of testing world.
TESTWORLD = babysfirst

### --- MAIN SECTION --- ###

all: tsunagari testworld

tsunagari: area.o common.o entity.o log.o main.o resourcer.o sprite.o window.o world.o
	$(CXX) -o tsunagari *.o $(LDFLAGS)

testworld: $(TESTWORLD)/*
	cd $(TESTWORLD) && zip ../$(TESTWORLD).world *

clean:
	$(RM) tsunagari *.o $(TESTWORLD).world

