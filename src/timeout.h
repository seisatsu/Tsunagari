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
	Timeout(boost::python::object callback, int delay);
	void cancel();

	bool isActive();
	bool ready(int now);
	int readyTime();
	void execute();

private:
	boost::python::object callback;
	int start, delay;
	bool active;
};

void updateTimeouts();
void exportTimeout();

#endif

