/****************************
** Tsunagari Tile Engine   **
** timer.cpp               **
** Copyright 2012 OmegaSDG **
****************************/

#include <boost/format.hpp>

#include "python.h"
#include "timer.h"
#include "window.h"

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
	if (running == true) {
		this->running = true;
		prev_time = GameWindow::instance().time();
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
	int prev_count = this->prev_count;

	if (running == true) {
		int now = GameWindow::instance().time();

		if (now > prev_time)
			prev_count = prev_count + (now - prev_time);

		else // Gosu::milliseconds() has overflowed; compensate.
			prev_count = prev_count + (prev_time - now);
	}

	return (double)prev_count / 1000.0;
}

double Timer::count()
{
	if (running == true) {
		int now = GameWindow::instance().time();

		if (now > prev_time)
			prev_count = prev_count + (now - prev_time);

		else // Gosu::milliseconds() has overflowed; compensate.
			prev_count = prev_count + (prev_time - now);

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

