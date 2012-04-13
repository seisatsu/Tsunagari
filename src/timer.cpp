/****************************
** Tsunagari Tile Engine   **
** timer.cpp               **
** Copyright 2012 OmegaSDG **
****************************/

#include <Gosu/Timing.hpp>

#include "python.h"
#include "timer.h"

static Timer pythonNewTimer()
{
	return Timer();
}

/*
 * Timer
 */

Timer::Timer()
	: running(false), prev_count(0)
{
}

bool Timer::isRunning()
{
	return running;
}

void Timer::setRunning(bool running)
{
	if (running == true) {
		this->running = true;
		prev_time = Gosu::milliseconds();
	}

	else
		this->running = false;
}

void Timer::reset()
{
	prev_count = 0;
}

double Timer::count()
{
	if (running == true) {
		unsigned long millis = Gosu::milliseconds();

		if (millis > prev_time)
			prev_count = prev_count + (millis - prev_time);

		else // Gosu::milliseconds() has overflowed; compensate.
			prev_count = prev_count + (prev_time - millis);

		prev_time = millis;
	}

	return (double)prev_count / 1000.0;
}

void exportTimer()
{
	using namespace boost::python;

	class_<Timer> ("Timer", no_init)
		.add_property("running", &Timer::isRunning, &Timer::setRunning)
		.add_property("count", &Timer::count)
		.def("reset", &Timer::reset)
		;

	pythonAddFunction("newTimer", pythonNewTimer);
}

