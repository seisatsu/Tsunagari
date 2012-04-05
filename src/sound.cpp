/*********************************
** Tsunagari Tile Engine        **
** sound.cpp                    **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include "python.h"
#include "python_optional.h"
#include "resourcer.h"
#include "sound.h"

SoundInstance::SoundInstance(Gosu::SampleInstance inst)
	: inst(inst), volume(1.0), pan(0.0), speed(1.0)
{
}


bool SoundInstance::isPlaying()
{
	return inst.playing();
}

void SoundInstance::stop()
{
	inst.stop();
}


bool SoundInstance::isPaused()
{
	return inst.paused();
}

void SoundInstance::setPaused(bool paused)
{
	if (paused)
		inst.pause();
	else
		inst.resume();
}


double SoundInstance::getVolume()
{
	return volume;
}

void SoundInstance::setVolume(double volume)
{
	this->volume = volume;
	inst.changeVolume(volume);
}


double SoundInstance::getPan()
{
	return pan;
}

void SoundInstance::setPan(double pan)
{
	this->pan = pan;
	inst.changePan(pan);
}


double SoundInstance::getSpeed()
{
	return speed;
}

void SoundInstance::setSpeed(double speed)
{
	this->speed = speed;
	inst.changeSpeed(speed);
}




Sound::Sound(Gosu::Sample* source)
	: source(source)
{
}

SoundInstance Sound::play()
{
	SoundInstance inst(source->play());
	return inst;
}

// Helper class for Python.
class SoundManager
{
public:
	boost::optional<SoundInstance> play(const std::string& path);
};

boost::optional<SoundInstance> SoundManager::play(const std::string& path)
{
	Resourcer* rc;
	boost::optional<SoundInstance> instance;
	SampleRef sample;

	rc = Resourcer::instance();
	sample = rc->getSample(path);
	if (sample)
		instance.reset(sample->play());
	return instance;
}

void exportSound()
{
	boost::python::class_<SoundInstance>
		("SoundInstance", boost::python::no_init)
		.add_property("paused", &SoundInstance::isPaused, &SoundInstance::setPaused)
		.add_property("volume", &SoundInstance::getVolume, &SoundInstance::setVolume)
		.add_property("pan", &SoundInstance::getPan, &SoundInstance::setPan)
		.add_property("speed", &SoundInstance::getSpeed, &SoundInstance::setSpeed)
		.def("playing", &SoundInstance::isPlaying)
		.def("stop", &SoundInstance::stop)
		;
	boost::python::optional_<SoundInstance>();
	boost::python::class_<SoundManager>
		("SoundManager", boost::python::no_init)
		.def("play", &SoundManager::play)
		;
	pythonSetGlobal("Sound", new SoundManager);
}

