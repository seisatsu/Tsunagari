/****************************
** Tsunagari Tile Engine   **
** timeout.h               **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <boost/python.hpp>

class Timeout {
public:
	Timeout(boost::python::object callback, int milliseconds);
	void cancel();

	bool isActive();
	bool ready(int now);
	void execute();

private:
	boost::python::object callback;
	int start, duration;
	bool active;
};

void updateTimeouts();
void exportTimeout();

#endif

