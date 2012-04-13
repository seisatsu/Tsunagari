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

static Timeout* pythonSetTimeout(boost::python::object callback, int milliseconds)
{
	Timeout* t = new Timeout(callback, milliseconds);
	timeouts.push_back(t);
	return t;
}

Timeout::Timeout(boost::python::object callback, int milliseconds)
	: callback(callback),
	  start(GameWindow::instance().time()),
	  duration(milliseconds),
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
	return now > start + duration;
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
	bool next = true;

	if (!timeouts.size())
		return;

	int now = GameWindow::instance().time();

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

	pythonAddFunction("timeout", make_function(
		pythonSetTimeout,
		return_value_policy<reference_existing_object>()
	));
}

