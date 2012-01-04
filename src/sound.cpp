/*********************************
** Tsunagari Tile Engine        **
** sound.cpp                    **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <boost/shared_ptr.hpp>

#include "python.h"
#include "sound.h"

Sample::Sample(Gosu::Sample* source)
	: source(source)
{
}

void Sample::play()
{
	source->play();
}


void exportSound()
{
	boost::python::class_<Sample, boost::shared_ptr<Sample> >
		("Sample", boost::python::no_init)
		.def("play", &Sample::play);
}

