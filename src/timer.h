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
	/*!
		@return true if running, false if stopped.
	*/
	bool isRunning() const;

	//! Set whether the timer is running.
	/*!
		@param running true to start the timer, false to stop it.
	*/
	void setRunning(bool running);

	//! Reset the timer to zero.
	void reset();

	//! Return the timer's count in seconds.
	double count() const;
	double count();

	//! Return a rough string representation of this object.
	std::string repr() const;

private:
	bool running;
	int prev_time;
	int prev_count;
};

void exportTimer();

#endif

