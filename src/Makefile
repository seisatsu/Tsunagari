CXXFLAGS = -g -Wall -Wextra -Werror -pedantic -ansi
LDFLAGS = -g

all: tsu

tsu: area.o entity.o main.o resourcer.o sprite.o window.o world.o
	$(CXX) -o tsu *.o -lgosu $(LDFLAGS)

clean:
	$(RM) *.o tsu

