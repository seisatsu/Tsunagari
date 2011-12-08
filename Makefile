# Top level Makefile for Tsunagari

all: debug

debug:
	$(MAKE) -C src all BLDCFLAGS=-g

release:
	$(MAKE) -C src all BLDCFLAGS=-O2

profile:
	$(MAKE) -C src all BLDCFLAGS=-pg BLDLDFLAGS=-pg

# Need to specify manually because './doc/' is a directory.
.PHONY: doc
doc:
	cd doc && doxygen

clean:
	$(MAKE) -C src clean
	$(RM) -r doc/html

