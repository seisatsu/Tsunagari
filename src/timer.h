/****************************
** Tsunagari Tile Engine   **
** timer.h                 **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef TIMER_H
#define TIMER_H

// Note: This will break if run long enough for the counter to overflow.

//! A timer class for timing Python events.
class Timer {
public:
	Timer();

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

