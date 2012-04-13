/****************************
** Tsunagari Tile Engine   **
** timeout.cpp             **
** Copyright 2012 OmegaSDG **
****************************/

#include <list>

#include "python.h"
#include "timeout.h"
#include "window.h"

static std::list<Timeout*> timeouts;

static Timeout* pythonSetTimeout(boost::python::object callback, int delay)
{
	int now = GameWindow::instance().time();
	int end = now + delay;

	// Insert sorted by resolution time.
	std::list<Timeout*>::iterator it;
	for (it = timeouts.begin(); it != timeouts.end(); it++) {
		Timeout* t = *it;
		if (end < t->readyTime())
			break;
	}

	Timeout* t = new Timeout(callback, delay);
	timeouts.insert(it, t);
	return t;
}

Timeout::Timeout(boost::python::object callback, int delay)
	: callback(callback),
	  start(GameWindow::instance().time()),
	  delay(delay),
	  active(true)
{
}

void Timeout::cancel()
{
	active = false;
}

bool Timeout::isActive()
{
	return active;
}

bool Timeout::ready(int now)
{
	return now > start + delay;
}

int Timeout::readyTime()
{
	return start + delay;
}

void Timeout::execute()
{
	try {
		callback();
	} catch (boost::python::error_already_set) {
		pythonErr();
	}
}

void updateTimeouts()
{
	int now = GameWindow::instance().time();
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
		;

	pythonAddFunction("timeout", make_function(pythonSetTimeout,
		return_value_policy<reference_existing_object>()));
}

