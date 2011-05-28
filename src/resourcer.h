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

#include "window.h"

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

	//! Returns the name of the file that this Resourcer is tied to. XXX (What? There's only one.)
	const std::string getFilename();

	//! Returns an image resource from disk or cache.
	Gosu::Image* getImage(const std::string& name);

	//! Returns a string resource from disk or cache.
	std::string getString(const std::string& name);

	//! Returns an XML resource from disk or cache.
	xmlNode* getXMLDoc(const std::string& name);

private:
	bool read(const std::string& name, Gosu::Buffer* buffer);
	std::string path(const std::string& entry_name);
	
	GameWindow* window;
	zip* z;
	std::string zip_filename;
};

std::string rcCurrentXmlFile;

#endif

