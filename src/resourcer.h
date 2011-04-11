/******************************
** Tsunagari Tile Engine     **
** resourcer.h               **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef RESOURCER_H
#define RESOURCER_H

#include <string>

#include <Gosu/Gosu.hpp>

//#include "event.h"
//#include "tile.h"
#include "window.h"

class GameWindow;

class Resourcer
{
public:
	Resourcer(GameWindow* window, const std::string& filename);
	~Resourcer();

	Gosu::Image* getImage(const std::string& name);
	std::string getString(const std::string& name);
//	Gosu::Font getFont(string name, float size);
//	vector<Tile*> getTiles(string name, int tileSize);
//	list<string> getAreaList();
//	YAML* getArea(string name);
//	Event* getEvent(string name);
//	Gosu::Music getMusic(string name);
//	Gosu::Sound getSound(string name);

private:
	void read(const std::string& name, Gosu::Buffer* buffer);

//	vector<byte>* getFile(string name);
//	Magick::ImageList* createImageList(string name);

	GameWindow* window;
	std::string filename;
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

