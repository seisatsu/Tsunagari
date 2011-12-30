/******************************
** Tsunagari Tile Engine     **
** sound.h                   **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef SAMPLE_H
#define SAMPLE_H

#include <Gosu/Audio.hpp>

class Sample
{
public:
	Sample(Gosu::Sample* source);
	void play();

private:
	Gosu::Sample* source;
};

void exportSound();

#endif

