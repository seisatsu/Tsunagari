/*********************************
** Tsunagari Tile Engine        **
** music.cpp                    **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include "music.h"
#include "python.h"

Music::Music()
	: volume(1.0),
	  paused(false),
	  state(NOT_PLAYING)
{
	pythonSetGlobal("Music", this);
}

Music::~Music()
{
	if (musicInst && musicInst->playing())
		musicInst->stop();
}

std::string Music::getIntro()
{
	return newIntro;
}

std::string Music::getLoop()
{
	return newLoop;
}

void Music::setIntro(const std::string& filename)
{
	if (newIntro == filename)
		return;

	switch (state) {
	case NOT_PLAYING:
	case PLAYING_INTRO:
	case PLAYING_LOOP:
		setState(CHANGED_INTRO);
	default: break;
	}

	Resourcer* rc = Resourcer::instance();

	newIntro = filename;
	// Optimize XXX: Don't load until played.
	introMusic = filename.size() ?
		rc->getSong(filename) : SongRef();
}

void Music::setLoop(const std::string& filename)
{
	if (newLoop == filename)
		return;

	switch (state) {
	case NOT_PLAYING:
	case PLAYING_INTRO:
	case PLAYING_LOOP:
		setState(CHANGED_LOOP);
	default: break;
	}

	Resourcer* rc = Resourcer::instance();

	newLoop = filename;
	// Optimize XXX: Don't load until played.
	loopMusic = filename.size() ?
		rc->getSong(filename) : SongRef();
}

double Music::getVolume()
{
	return volume;
}

void Music::setVolume(double level)
{
	volume = level;
	if (musicInst)
		musicInst->changeVolume(level);
}

bool Music::isPaused()
{
	return paused;
}

void Music::setPaused(bool p)
{
	if (paused == p)
		return;
	paused = p;
	if (musicInst) {
		if (p)
			musicInst->pause();
		else
			musicInst->play();
	}
}

void Music::stop()
{
	paused = false;
	if (musicInst)
		musicInst->stop();
	state = NOT_PLAYING;
}

void Music::tick()
{
	if (paused)
		return;

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
	introMusic->changeVolume(volume);
	musicInst = introMusic;
	setState(PLAYING_INTRO);
}

void Music::playLoop()
{
	if (musicInst && musicInst->playing())
		musicInst->stop();
	curLoop = newLoop;
	loopMusic->play(true);
	loopMusic->changeVolume(volume);
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

void exportMusic()
{
	boost::python::class_<Music>("MusicManager", boost::python::no_init)
		.add_property("intro", &Music::getIntro, &Music::setIntro)
		.add_property("loop", &Music::getLoop, &Music::setLoop)
		.add_property("volume", &Music::getVolume, &Music::setVolume)
		.add_property("paused", &Music::isPaused, &Music::setPaused)
		.def("stop", &Music::stop)
		;
}
