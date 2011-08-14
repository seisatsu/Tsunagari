# Top level Makefile for Tsunagari

all: debug

debug:
	cd deps/lua && make all MYCFLAGS=-g
	cd src && make all MYCFLAGS=-g

release:
	cd deps/lua && make all MYCFLAGS=-O2
	cd src && make all MYCFLAGS=-O2

profile:
	cd deps/lua && make all MYCFLAGS=-pg
	cd src && make all MYCFLAGS=-pg MYLDFLAGS=-pg

# Need to specify manually because './doc/' is a directory.
.PHONY: doc
doc:
	cd doc && doxygen

clean:
	cd deps/lua && make clean
	cd src && make clean
	$(RM) -r doc/html

