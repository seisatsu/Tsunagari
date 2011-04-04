CXXFLAGS = -g -Wall -Wextra -Werror -pedantic -ansi

all: tsu

tsu: entity.o main.o resourcer.o sprite.o window.o descriptor.o common.o
	$(CXX) -g -o tsu *.o -lgosu -lyaml

clean:
	$(RM) *.o tsu

