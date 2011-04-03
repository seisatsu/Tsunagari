CXXFLAGS = -g -Wall -Wextra -Werror -pedantic -ansi
LDFLAGS = -g -lgosu

all: tsu

tsu: area.o entity.o main.o resourcer.o sprite.o window.o world.o
	$(CXX) -o tsu *.o $(LDFLAGS)

clean:
	$(RM) tsu *.o

