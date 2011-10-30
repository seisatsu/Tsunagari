/******************************
** Tsunagari Tile Engine     **
** music.cpp                 **
** Copyright 2011 OmegaSDG   **
******************************/

#include "music.h"

Music::Music(Resourcer* rc) : rc(rc), state(NOT_PLAYING)
{
}

Music::~Music()
{
}

void Music::setIntro(const std::string& filename)
{
	switch (state) {
	case NOT_PLAYING:
	case PLAYING_INTRO:
	case PLAYING_MAIN:
		if (newIntro != filename)
			setState(CHANGED_INTRO);
	default:
		break;
	}
	if (newIntro != filename) {
		newIntro = filename;
		introMusic = filename.size() ?
			rc->getSample(filename) : SampleRef();
	}
}

void Music::setMain(const std::string& filename)
{
	switch (state) {
	case NOT_PLAYING:
	case PLAYING_INTRO:
	case PLAYING_MAIN:
		if (newMain != filename)
			setState(CHANGED_MAIN);
	default:
		break;
	}
	if (newMain != filename) {
		newMain = filename;
		mainMusic = filename.size() ?
			rc->getSample(filename) : SampleRef();
	}
}

void Music::update()
{
	switch (state) {
	case NOT_PLAYING:
		if (musicInst->playing())
			musicInst->stop();
		break;
	case PLAYING_INTRO:
		if (!musicInst->playing()) {
			if (newMain.size())
				playMain();
			else
				setState(NOT_PLAYING);
		}
		break;
	case PLAYING_MAIN:
		break;
	case CHANGED_INTRO:
		if (newIntro.size())
			playIntro();
		else if (newMain.size() && newMain != curMain)
			setState(CHANGED_MAIN);
		else if (newMain.size())
			setState(PLAYING_MAIN);
		else
			setState(NOT_PLAYING);
		break;
	case CHANGED_MAIN:
		if (newIntro.size())
			playIntro();
		else if (newMain.size())
			playMain();
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
	musicInst.reset(introMusic->play(1, 1, false));
	setState(PLAYING_INTRO);
}

void Music::playMain()
{
	if (musicInst && musicInst->playing())
		musicInst->stop();
	curMain = newMain;
	musicInst.reset(mainMusic->play(1, 1, true));
	setState(PLAYING_MAIN);
}

/*
static const char* stateStr(MUSIC_STATE state)
{
	switch (state) {
	case NOT_PLAYING:
		return "NOT_PLAYING";
	case PLAYING_INTRO:
		return "PLAYING_INTRO";
	case PLAYING_MAIN:
		return "PLAYING_MAIN";
	case CHANGED_INTRO:
		return "CHANGED_INTRO";
	case CHANGED_MAIN:
		return "CHANGED_MAIN";
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

