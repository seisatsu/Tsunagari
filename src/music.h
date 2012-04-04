/*********************************
** Tsunagari Tile Engine        **
** music.h                      **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef MUSIC_H
#define MUSIC_H

#include <boost/optional/optional.hpp>
#include <Gosu/Audio.hpp> // for Gosu::SampleInstance

#include "resourcer.h"

enum MUSIC_STATE
{
	NOT_PLAYING,
	PLAYING_INTRO,
	PLAYING_LOOP,
	CHANGED_INTRO,
	CHANGED_LOOP
};

/**
 * State manager for currently playing music. Continuously controls which music
 * will play. At the moment, each Area has an INTRO music and a LOOP music.
 * Immediately upon entering an Area, the currently playing music is stopped
 * and the associated intro music started. Once the intro music finishes, or,
 * if there is no intro music, then immediately upon entering said Area, the
 * loop music begins. Loop music is looped forever until either the world is
 * exited or new music is specified, either by a script or by entering a new
 * Area.
 *
 * When switching to a new Area with the same intro or loop music as the
 * previous Area, the music is left alone, if possible.
 */
class Music
{
public:
	Music();
	~Music();

	std::string getIntro();
	std::string getLoop();

	void setIntro(const std::string& filename);
	void setLoop(const std::string& filename);

	double getVolume();
	void setVolume(double level);

	bool getPaused();
	void setPaused(bool p);

	void stop();

	void update();

private:
	void playIntro();
	void playLoop();
	void setState(MUSIC_STATE state);

	SongRef musicInst, introMusic, loopMusic;

	double volume;
	bool paused;

	MUSIC_STATE state;

	std::string newIntro;
	std::string newLoop;
	std::string curIntro;
	std::string curLoop;
};

//! Register Music with Python.
void exportMusic();

#endif

