CXXFLAGS = -g -Wall -Wextra -pedantic -ansi
LDFLAGS = -g -lgosu -ljson -lzip

all: tsu

tsu: area.o common.o entity.o log.o main.o resourcer.o sprite.o window.o world.o
	$(CXX) -o tsu *.o $(LDFLAGS)

clean:
	$(RM) tsu *.o

