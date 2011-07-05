# TLD Makefile - Aliases to src/Makefile

all:
	cd src && make all

tsunagari:
	cd src && make tsunagari

testworld:
	cd src && make testworld

.PHONY: doc
doc:
	cd src && make doc

clean:
	cd src && make clean

