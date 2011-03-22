/****************************
** Tsunagari Tile Engine   **
** window.h - Window       **
** Copyright 2011 OmegaSDG **
****************************/

#include "resourcer.h"
#include "world.h"

class Window : public Gosu::Window
{
public:
	Window(hash_t params);
	void button_down(int id);
	void draw();
	bool needs_redraw();

private:
	string get_datadir();
};

