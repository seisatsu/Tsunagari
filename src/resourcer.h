/******************************
** Tsunagari Tile Engine     **
** resourcer.h               **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef RESOURCER_H
#define RESOURCER_H

#include <string>

#include <Gosu/Gosu.hpp>
#include <zip.h>

#include "window.h"

class GameWindow;

class Resourcer
{
public:
	Resourcer(GameWindow* window, const std::string& filename);
	~Resourcer();

	bool init();

	/**
	 * Returns the name of the file that this Resourcer is tied to.
	 */
	const std::string getFilename();

	Gosu::Image* getImage(const std::string& name);
	std::string getString(const std::string& name);

private:
	bool read(const std::string& name, Gosu::Buffer* buffer);
	std::string path(const std::string& entry_name);

	GameWindow* window;
	zip* z;
	std::string zip_filename;
};

#endif

