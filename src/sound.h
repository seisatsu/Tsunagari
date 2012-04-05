/*********************************
** Tsunagari Tile Engine        **
** sound.h                      **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef SOUND_H
#define SOUND_H

namespace Gosu {
	class Sample;
}

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

