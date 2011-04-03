CXXFLAGS = -g -Wall -Wextra -Werror -pedantic -ansi

all: tsu

tsu: entity.o main.o resourcer.o sprite.o window.o
	$(CXX) -g -o tsu *.o -lgosu

clean:
	$(RM) *.o tsu

