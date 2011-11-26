# Top level Makefile for Tsunagari

all: debug

debug:
	$(MAKE) -C src all MYCFLAGS=-g

release:
	$(MAKE) -C src all MYCFLAGS=-O2

profile:
	$(MAKE) -C src all MYCFLAGS=-pg MYLDFLAGS=-pg

# Need to specify manually because './doc/' is a directory.
.PHONY: doc
doc:
	cd doc && doxygen

clean:
	$(MAKE) -C src clean
	$(RM) -r doc/html

