/***************************************
** Tsunagari Tile Engine              **
** sound.cpp                          **
** Copyright 2011-2013 PariahSoft LLC **
***************************************/

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// **********

#include <Gosu/Math.hpp>

#include "client-conf.h"
#include "python.h"
#include "python-bindings-template.cpp"
#include "reader.h"
#include "sound.h"

SoundInstance::SoundInstance(Gosu::SampleInstance inst)
	: inst(inst), volume(conf.soundVolume), pan(0.0), speed(1.0)
{
	inst.changeVolume(volume / 100.0);
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


int SoundInstance::getVolume()
{
	return volume;
}

void SoundInstance::setVolume(int volume)
{
	if (0 < volume || volume > 100) {
		Log::info("SoundInstance", "volume can only be set between 0 and 100");
		volume = Gosu::clamp(volume, 0, 100);
	}
	this->volume = volume;
	inst.changeVolume(volume / 100.0);
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
	return SoundInstance(source->play());
}

// Helper class for Python.
class SoundManager
{
public:
	SoundInstanceRef play(const std::string& path);
};

SoundInstanceRef SoundManager::play(const std::string& path)
{
	SampleRef sample;

	sample = Reader::getSample(path);
	if (sample)
		return SoundInstanceRef(new SoundInstance(sample->play()));
	else
		return SoundInstanceRef(NULL);
}

void exportSound()
{
	using namespace boost::python;

	class_<SoundInstance, SoundInstanceRef>
		("SoundInstance", no_init)
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
	class_<SoundManager>
		("SoundManager", no_init)
		.def("play", &SoundManager::play)
		;
	pythonSetGlobal("Sound", new SoundManager);
}

