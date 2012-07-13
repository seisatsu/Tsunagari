/****************************
** Tsunagari Tile Engine   **
** timeout.h               **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <string>

#include <boost/python.hpp>

class Timeout {
public:
	Timeout(boost::python::object callback, time_t delay);
	void cancel();

	bool isActive() const;
	bool ready(time_t now) const;
	time_t readyTime() const;
	void execute();

	//! Return a rought string representation of this object.
	std::string repr() const;

private:
	boost::python::object callback;
	time_t start, delay;
	bool active;
};

void updateTimeouts();
void exportTimeout();

#endif

