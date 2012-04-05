/*********************************
** Tsunagari Tile Engine        **
** sound.cpp                    **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <Gosu/Audio.hpp>

#include "python.h"
#include "python_optional.h"
#include "resourcer.h"
#include "sound.h"

Sound::Sound(Gosu::Sample* source)
	: source(source)
{
}

void Sound::play()
{
	source->play();
}

// Helper class for Python.
class SoundManager
{
public:
	boost::optional<Gosu::SampleInstance> play(const std::string& path);
};

boost::optional<Gosu::SampleInstance> SoundManager::play(const std::string& path)
{
	Resourcer* rc;
	boost::optional<Gosu::SampleInstance> instance;
	SampleRef sample;

	rc = Resourcer::instance();
	sample = rc->getSample(path);
	if (sample)
		instance.reset(sample->play());
	return instance;
}

void exportSound()
{
	boost::python::class_<Gosu::SampleInstance>
		("SoundInstance", boost::python::no_init)
		.def("playing", &Gosu::SampleInstance::playing)
		.def("paused", &Gosu::SampleInstance::paused)
		.def("pause", &Gosu::SampleInstance::pause)
		.def("resume", &Gosu::SampleInstance::resume)
		.def("stop", &Gosu::SampleInstance::stop)
		// setVolume
		// setPan
		// setSpeed
		;
	boost::python::optional_<Gosu::SampleInstance>();
	boost::python::class_<SoundManager>
		("SoundManager", boost::python::no_init)
		.def("play", &SoundManager::play)
		;
	pythonSetGlobal("Sound", new SoundManager);
}

