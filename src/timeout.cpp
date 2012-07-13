/****************************
** Tsunagari Tile Engine   **
** timeout.cpp             **
** Copyright 2012 OmegaSDG **
****************************/

#include <list>

#include <boost/format.hpp>

#include "python.h"
#include "timeout.h"
#include "world.h"

static std::list<Timeout*> timeouts;

static Timeout* pythonSetTimeout(boost::python::object callback, float delay)
{
	if (delay < 0.0)
		delay = 0.0;

	time_t delayi = (time_t)(1000 * delay);
	time_t now = World::instance()->time();
	time_t end = now + delayi;

	// Insert sorted by resolution time.
	std::list<Timeout*>::iterator it;
	for (it = timeouts.begin(); it != timeouts.end(); it++) {
		Timeout* t = *it;
		if (end < t->readyTime())
			break;
	}

	Timeout* t = new Timeout(callback, delayi);
	timeouts.insert(it, t);
	return t;
}

Timeout::Timeout(boost::python::object callback, time_t delay)
	: callback(callback),
	  start(World::instance()->time()),
	  delay(delay),
	  active(true)
{
}

void Timeout::cancel()
{
	active = false;
}

bool Timeout::isActive() const
{
	return active;
}

bool Timeout::ready(time_t now) const
{
	return now > start + delay;
}

time_t Timeout::readyTime() const
{
	return start + delay;
}

void Timeout::execute()
{
	try {
		inPythonScript++;
		callback();
		inPythonScript--;
	} catch (boost::python::error_already_set) {
		inPythonScript--;
		pythonErr();
	}
}

std::string Timeout::repr() const
{
	using namespace boost;

	time_t now = World::instance()->time();
	return str(format("<timeout time_remaining=%dms active=%s />")
			% (start + delay - now)
			% (isActive() ? "true" : "false")
	);
}

void updateTimeouts()
{
	time_t now = World::instance()->time();
	bool next = true;

	while (next && timeouts.size()) {
		Timeout* t = timeouts.front();
		if (!t->isActive()) {
			timeouts.pop_front();
			delete t;
		}
		else if (t->ready(now)) {
			t->execute();
			timeouts.pop_front();
			delete t;
		}
		else {
			next = false;
		}
	}
}

void exportTimeout()
{
	using namespace boost::python;

	class_<Timeout> ("Timeout", no_init)
		.def("cancel", &Timeout::cancel)
		.def("__repr__", &Timeout::repr)
		;

	pythonAddFunction("timeout", make_function(pythonSetTimeout,
		return_value_policy<reference_existing_object>()));
}

