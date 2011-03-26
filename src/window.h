/****************************
** Tsunagari Tile Engine   **
** window.h - Window       **
** Copyright 2011 OmegaSDG **
****************************/

//#include "resourcer.h"
//#include "world.h"
#include <Gosu/Gosu.hpp>

class GameWindow : public Gosu::Window
{
public:
	GameWindow();
	void button_down(int id);
	void draw();
	void update();
	bool needs_redraw();

private:
//	string get_datadir();
};

