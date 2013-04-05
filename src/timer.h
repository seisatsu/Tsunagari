/***************************************
** Tsunagari Tile Engine              **
** timer.h                            **
** Copyright 2011-2013 PariahSoft LLC **
***************************************/

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// **********

#ifndef TIMER_H
#define TIMER_H

#include <ctime> // for time_t
#include <string>

// Note: This will break if run long enough for the counter to overflow.

//! A timer class for timing Python events.
class Timer {
public:
	Timer();

	/**
	 * Check if the timer is running.
	 * @return true if running, false if stopped.
	 */
	bool isRunning() const;

	/**
	 * Set whether the timer is running.
	 * @param running true to start the timer, false to stop it.
	 */
	void setRunning(bool running);

	/**
	 * Reset the timer to zero.
	 */
	void reset();

	/**
	 * Return the timer's count in seconds.
	 *
	 */
	//@{
	double count();
	double count() const;
	//@}

	/**
	 * Return a rough string representation of this object.
	 */
	std::string repr() const;

private:
	bool running;
	time_t prev_time;
	time_t prev_count;
};

void exportTimer();

#endif

