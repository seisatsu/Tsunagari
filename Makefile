# Top level Makefile for Tsunagari

all: debug

debug:
	$(MAKE) -C src all BLDCFLAGS="-g"

release:
	$(MAKE) -C src all BLDCFLAGS="-O2 -flto" BLDLDFLAGS="-O2 -flto -s"

profile:
	$(MAKE) -C src all BLDCFLAGS="-pg" BLDLDFLAGS="-pg"

clean:
	$(MAKE) -C src clean

