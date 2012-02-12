/*********************************
** Tsunagari Tile Engine        **
** music.cpp                    **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include "music.h"

Music::Music()
	: state(NOT_PLAYING)
{
}

Music::~Music()
{
	if (musicInst && musicInst->playing())
		musicInst->stop();
}

void Music::setIntro(const std::string& filename)
{
	switch (state) {
	case NOT_PLAYING:
	case PLAYING_INTRO:
	case PLAYING_LOOP:
		if (newIntro != filename)
			setState(CHANGED_INTRO);
	default:
		break;
	}
	if (newIntro != filename) {
		newIntro = filename;
		introMusic = filename.size() ? Resourcer::getResourcer()->getSong(filename) : SongRef();
	}
}

void Music::setLoop(const std::string& filename)
{
	switch (state) {
	case NOT_PLAYING:
	case PLAYING_INTRO:
	case PLAYING_LOOP:
		if (newLoop != filename)
			setState(CHANGED_LOOP);
	default:
		break;
	}
	if (newLoop != filename) {
		newLoop = filename;
		loopMusic = filename.size() ? Resourcer::getResourcer()->getSong(filename) : SongRef();
	}
}

void Music::update()
{
	switch (state) {
	case NOT_PLAYING:
		if (musicInst && musicInst->playing())
			musicInst->stop();
		break;
	case PLAYING_INTRO:
		if (!musicInst->playing()) {
			if (newLoop.size() && loopMusic)
				playLoop();
			else
				setState(NOT_PLAYING);
		}
		break;
	case PLAYING_LOOP:
		break;
	case CHANGED_INTRO:
		if (newIntro.size() && introMusic)
			playIntro();
		else if (newLoop.size() && newLoop != curLoop)
			setState(CHANGED_LOOP);
		else if (newLoop.size())
			setState(PLAYING_LOOP);
		else
			setState(NOT_PLAYING);
		break;
	case CHANGED_LOOP:
		if (newIntro.size() && loopMusic)
			playIntro();
		else if (newLoop.size() && loopMusic)
			playLoop();
		else
			setState(NOT_PLAYING);
		break;
	}
}

void Music::playIntro()
{
	if (musicInst && musicInst->playing())
		musicInst->stop();
	curIntro = newIntro;
	introMusic->play(false);
	musicInst = introMusic;
	setState(PLAYING_INTRO);
}

void Music::playLoop()
{
	if (musicInst && musicInst->playing())
		musicInst->stop();
	curLoop = newLoop;
	loopMusic->play(true);
	musicInst = loopMusic;
	setState(PLAYING_LOOP);
}

/*
static const char* stateStr(MUSIC_STATE state)
{
	switch (state) {
	case NOT_PLAYING:
		return "NOT_PLAYING";
	case PLAYING_INTRO:
		return "PLAYING_INTRO";
	case PLAYING_LOOP:
		return "PLAYING_LOOP";
	case CHANGED_INTRO:
		return "CHANGED_INTRO";
	case CHANGED_LOOP:
		return "CHANGED_LOOP";
	default:
		return "";
	}
}
*/

void Music::setState(MUSIC_STATE state)
{
	// printf("State changed from %s to %s.\n", stateStr(this->state), stateStr(state));
	this->state = state;
}

