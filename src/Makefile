CXXFLAGS = -g -Wall -Wextra -pedantic -ansi `xml2-config --cflags`
LDFLAGS = -g -lgosu -lzip `xml2-config --libs`

all: tsu babysfirst.world

tsu: area.o common.o entity.o log.o main.o resourcer.o sprite.o window.o world.o
	$(CXX) -o tsu *.o $(LDFLAGS)

babysfirst.world: babysfirst/*
	cd babysfirst && zip ../babysfirst.world *

clean:
	$(RM) tsu *.o babysfirst.world

