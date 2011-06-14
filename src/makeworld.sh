#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Usage: `basename $0` <world>"
	exit 1
fi

cd $1 && zip ../$1.world ./*

