/****************************
** Tsunagari Tile Engine   **
** resourcer.h - Resourcer **
** Copyright 2011 OmegaSDG **
****************************/

#include "event.h"
#include "tile.h"
#include "window.h"

class Resourcer
{
public:
	Resourcer(Window* window, string filename);
	Gosu::Font get_font(string name, float size);
	Gosu::Image get_image(string name);
	vector<Tile*> get_tiles(string name, int tile_size);
	list<string> get_area_list();
	YAML* get_area(string name);
	Event* get_event(string name);
	Gosu::Music get_music(string name);
	Gosu::Sound get_sound(string name);
	
private:
	vector<byte>* get_file(string name);
	Magick::ImageList* create_imagelist(string name);

	Window* window;
	list<libarchive_entry> files;
	
	map<string,Gosu::Font> fonts;
	map<string,Gosu::Image> images;
	map<string,Tile> tiles;
	map<string,Event> events;
	map<string,Gosu::Music> music;
	map<string,Gosu::Sound> snds;
};

