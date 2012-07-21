#!/bin/sh

exec > all

for f in *.h *.cpp; do
	egrep "^#include" $f
done
