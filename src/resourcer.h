/******************************
** Tsunagari Tile Engine     **
** resourcer.h - Resourcer   **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef RESOURCER_H
#define RESOURCER_H

#include <string>

//#include "event.h"
//#include "tile.h"
#include "window.h"

class GameWindow;

class Resourcer
{
public:
	Resourcer(GameWindow* window/*, string filename*/);
	~Resourcer();

//	Gosu::Font getFont(string name, float size);
	Gosu::Image* getImage(const std::string name);
//	vector<Tile*> getTiles(string name, int tileSize);
//	list<string> getAreaList();
//	YAML* getArea(string name);
//	Event* getEvent(string name);
//	Gosu::Music getMusic(string name);
//	Gosu::Sound getSound(string name);
	
private:
//	vector<byte>* getFile(string name);
//	Magick::ImageList* createImageList(string name);

	GameWindow* window;
//	list<libarchive_entry> files;
	
/*
	map<string,Gosu::Font> fonts;
	map<string,Gosu::Image> images;
	map<string,Tile> tiles;
	map<string,Event> events;
	map<string,Gosu::Music> music;
	map<string,Gosu::Sound> snds;
*/
};

#endif

