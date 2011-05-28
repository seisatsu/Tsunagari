CXXFLAGS = -g -Wall -Wextra -pedantic -ansi `xml2-config --cflags`
LDFLAGS = -g -lgosu -lzip `xml2-config --libs`

all: tsu testworld

tsu: area.o common.o entity.o log.o main.o resourcer.o sprite.o tile.o tilematrix.o window.o world.o
	$(CXX) -o tsu *.o $(LDFLAGS)

testworld:
	cd babysfirst && zip ../babysfirst.world *

clean:
	$(RM) tsu *.o babysfirst.world

