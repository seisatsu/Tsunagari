/*********************************
** Tsunagari Tile Engine        **
** timer.cpp                    **
** Copyright 2011-2012 OmegaSDG **
*********************************/

// "OmegaSDG" is defined as Michael D. Reiley and Paul Merrill.

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

#include <boost/format.hpp>

#include "python.h"
#include "timer.h"
#include "world.h"

static Timer pythonNewTimer()
{
	return Timer();
}


Timer::Timer()
	: running(false), prev_count(0)
{
}

bool Timer::isRunning() const
{
	return running;
}

void Timer::setRunning(bool running)
{
	if (running) {
		this->running = true;
		prev_time = World::instance()->time();
	}

	else
		this->running = false;
}

void Timer::reset()
{
	prev_count = 0;
}

double Timer::count() const
{
	time_t prev_count = this->prev_count;

	if (running) {
		time_t now = World::instance()->time();
		prev_count = prev_count + (now - prev_time);
	}

	return (double)prev_count / 1000.0;
}

double Timer::count()
{
	if (running) {
		time_t now = World::instance()->time();
		prev_count = prev_count + (now - prev_time);
		prev_time = now;
	}

	return (double)prev_count / 1000.0;
}

std::string Timer::repr() const
{
	using namespace boost;

	return str(format("<timer count=%.02fsec running=%s />")
			% count() % (isRunning() ? "true" : "false"));
}

void exportTimer()
{
	using namespace boost::python;

	class_<Timer> ("Timer", no_init)
		.add_property("running", &Timer::isRunning, &Timer::setRunning)
		.add_property("count",
			static_cast<double (Timer::*) ()> (&Timer::count))
		.def("reset", &Timer::reset)
		.def("__repr__", &Timer::repr)
		;

	pythonAddFunction("new_timer", pythonNewTimer);
}

