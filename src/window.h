/*********************************
** Tsunagari Tile Engine        **
** window.h                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

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

#ifndef WINDOW_H
#define WINDOW_H

#include <map>
#include <string>

#include <boost/scoped_ptr.hpp>
#include <Gosu/Window.hpp> // for Gosu::Window

namespace Gosu {
	class Button;
}

class Resourcer;
class World;

//! GameWindow Class
/*!
	This class is structurally the main class of the Tsunagari Tile Engine.
	It handles input and drawing.
*/
class GameWindow : public Gosu::Window
{
public:
	static GameWindow& instance();

	//! GameWindow Constructor
	GameWindow();

	//! GameWindow Destructor
	virtual ~GameWindow();

	//! GameWindow Initializer
	bool init(char* argv0);

	//! Width of the window in pixels.
	int width() const;

	//! Height of the window in pixels.
	int height() const;

	//! Gosu Callback
	void buttonDown(const Gosu::Button btn);

	//! Gosu Callback
	void buttonUp(const Gosu::Button btn);

	//! Gosu Callback
	void draw();

	//! Gosu Callback
	bool needsRedraw() const;

	//! Gosu Callback
	void update();

	//! Time since epoch.
	time_t time() const;

protected:
	//! Process persistent keyboard input
	void handleKeyboardInput(time_t now);

	boost::scoped_ptr<Resourcer> rc;
	boost::scoped_ptr<World> world;

	time_t now;
	time_t lastGCtime;

	struct keystate {
		bool consecutive, initiallyResolved;
		time_t since;
	};

	std::map<Gosu::Button, keystate> keystates;
};

#endif

