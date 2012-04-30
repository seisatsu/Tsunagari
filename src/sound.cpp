/*********************************
** Tsunagari Tile Engine        **
** sound.cpp                    **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <boost/shared_ptr.hpp>

#include "python.h"
#include "resourcer.h"
#include "sound.h"

SoundInstance::SoundInstance(boost::optional<Gosu::SampleInstance> inst)
	: inst(inst), volume(1.0), pan(0.0), speed(1.0)
{
}


bool SoundInstance::isPlaying()
{
	return inst && inst->playing();
}

void SoundInstance::stop()
{
	if (inst)
	       inst->stop();
}


bool SoundInstance::isPaused()
{
	return inst ? inst->paused() : false;
}

void SoundInstance::setPaused(bool paused)
{
	if (inst) {
		if (paused)
			inst->pause();
		else
			inst->resume();
	}
}


double SoundInstance::getVolume()
{
	return volume;
}

void SoundInstance::setVolume(double volume)
{
	this->volume = volume;
	if (inst)
		inst->changeVolume(volume);
}


double SoundInstance::getPan()
{
	return pan;
}

void SoundInstance::setPan(double pan)
{
	this->pan = pan;
	if (inst)
		inst->changePan(pan);
}


double SoundInstance::getSpeed()
{
	return speed;
}

void SoundInstance::setSpeed(double speed)
{
	this->speed = speed;
	if (inst)
		inst->changeSpeed(speed);
}




Sound::Sound(Gosu::Sample* source)
	: source(source)
{
}

SoundInstance Sound::play()
{
	return SoundInstance(source->play());
}

// Helper class for Python.
class SoundManager
{
public:
	SoundInstance play(const std::string& path);
};

SoundInstance SoundManager::play(const std::string& path)
{
	Resourcer* rc;
	SampleRef sample;

	rc = Resourcer::instance();
	sample = rc->getSample(path);
	if (sample)
		return sample->play();
	else
		return SoundInstance(NULL);
}

void exportSound()
{
	boost::python::class_<SoundInstance>
		("SoundInstance", boost::python::no_init)
		.add_property("paused",
		    &SoundInstance::isPaused, &SoundInstance::setPaused)
		.add_property("volume",
		    &SoundInstance::getVolume, &SoundInstance::setVolume)
		.add_property("pan",
		    &SoundInstance::getPan, &SoundInstance::setPan)
		.add_property("speed",
		    &SoundInstance::getSpeed, &SoundInstance::setSpeed)
		.add_property("playing", &SoundInstance::isPlaying)
		.def("stop", &SoundInstance::stop)
		;
	boost::python::class_<SoundManager>
		("SoundManager", boost::python::no_init)
		.def("play", &SoundManager::play)
		;
	pythonSetGlobal("Sound", new SoundManager);
}

