/*********************************
** Tsunagari Tile Engine        **
** sound.h                      **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef SOUND_H
#define SOUND_H

#include <Gosu/Audio.hpp>

class SoundInstance
{
public:
	SoundInstance(Gosu::SampleInstance inst);

	bool isPlaying();
	void stop();

	bool isPaused();
	void setPaused(bool paused);

	double getVolume();
	void setVolume(double volume);

	double getPan();
	void setPan(double pan);

	double getSpeed();
	void setSpeed(double speed);

private:
	Gosu::SampleInstance inst;
	double volume, pan, speed;
};

class Sound
{
public:
	Sound(Gosu::Sample* source);
	SoundInstance play();

private:
	Gosu::Sample* source;
};

void exportSound();

#endif

