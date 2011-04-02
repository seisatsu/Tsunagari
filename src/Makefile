all: tsu

tsu: entity.o main.o resourcer.o sprite.o window.o
	$(CXX) -o tsu *.o -lgosu

clean:
	$(RM) *.o tsu

