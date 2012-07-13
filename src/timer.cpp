/****************************
** Tsunagari Tile Engine   **
** timer.cpp               **
** Copyright 2012 OmegaSDG **
****************************/

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

