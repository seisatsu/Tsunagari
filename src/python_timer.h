/****************************
** Tsunagari Tile Engine   **
** python_timer.h          **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef PYTHON_TIMER_H
#define PYTHON_TIMER_H

// Note: This could glitch if run for several hours.

//! A timer class for timing Python events.
class Timer {
public:
	Timer();

	//! Start the timer.
	void start();

	//! Check if the timer is running.
	bool isRunning();

	//! Set whether the timer is running.
	void setRunning(bool running);

	//! Reset the timer to zero.
	void reset();

	//! Return the timer's count in seconds.
	double count();

private:
	bool running;
	unsigned long prev_time;
	unsigned long prev_count;
};

void exportTimer();

#endif

