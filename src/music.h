/******************************
** Tsunagari Tile Engine     **
** music.h                   **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef MUSIC_H
#define MUSIC_H

#include <boost/optional/optional.hpp>
#include <Gosu/Audio.hpp> // for Gosu::SampleInstance

#include "resourcer.h"

enum MUSIC_STATE
{
	NOT_PLAYING,
	PLAYING_INTRO,
	PLAYING_MAIN,
	CHANGED_INTRO,
	CHANGED_MAIN
};

class Music
{
public:
	//! Music Constructor
	Music(Resourcer* rc);

	//! Music Destructor
	~Music();

	void setIntro(const std::string& filename);
	void setMain(const std::string& filename);

	void update();

private:
	void playIntro();
	void playMain();
	void setState(MUSIC_STATE state);

	Resourcer* rc;

	SongRef musicInst, introMusic, mainMusic;

	MUSIC_STATE state;

	std::string newIntro;
	std::string newMain;
	std::string curIntro;
	std::string curMain;
};

#endif

