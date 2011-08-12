# TLD Makefile - Aliases to src/Makefile

all:

%:
	cd src && make $@

# Need to specify manually because './doc/' is a directory.
.PHONY: doc
doc:
	cd src && make doc

