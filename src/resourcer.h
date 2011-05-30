/******************************
** Tsunagari Tile Engine     **
** resourcer.h               **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef RESOURCER_H
#define RESOURCER_H

#include <string>

#include <Gosu/Gosu.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <zip.h>

class GameWindow;

//! Resourcer Class
/*!
	This class provides the engine's global resource handling and caching.
*/
class Resourcer
{
public:
	//! Resourcer Constructor
	Resourcer(GameWindow* window, const std::string& filename);

	//! Resourcer Destructor
	~Resourcer();

	//! Resourcer Initializer
	bool init();

	//! Returns an image resource from disk or cache.
	Gosu::Image* getImage(const std::string& name);

	//! Returns a string resource from disk or cache.
	std::string getString(const std::string& name);

	//! Returns an XML resource from disk or cache.
	xmlDoc* getXMLDoc(const std::string& name);

	//! Returns a music stream from disk or cache.
	Gosu::Sample* getSample(const std::string& name);

private:
	//! Read a resource from disk into memory. Returns NULL on error.
	Gosu::Buffer* read(const std::string& name);

	//! Helper function 
	std::string path(const std::string& entry_name) const;

	GameWindow* window;
	zip* z;
	const std::string zip_filename;
};

#endif

