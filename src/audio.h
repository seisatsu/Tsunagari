/*********************************
** Tsunagari Tile Engine        **
** audio.h                      **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef AUDIO_H
#define AUDIO_H

#include <Gosu/Audio.hpp>

class Sound
{
public:
	Sound(Gosu::Sample* source);
	void play();

private:
	Gosu::Sample* source;
};

void exportSound();

#endif

