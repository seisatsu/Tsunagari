CXXFLAGS = -g -Wall -Wextra -pedantic -ansi `xml2-config --cflags`
LDFLAGS = -g -lgosu -ljson -lzip `xml2-config --libs`

all: tsu

tsu: area.o common.o entity.o log.o main.o resourcer.o sprite.o tile.o tilematrix.o window.o world.o
	$(CXX) -o tsu *.o $(LDFLAGS)

clean:
	$(RM) tsu *.o

