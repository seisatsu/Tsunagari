/******************************
** Tsunagari Tile Engine     **
** resourcer.h               **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef RESOURCER_H
#define RESOURCER_H

#include <string>
#include <utility>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "common.h"

struct zip;

namespace Gosu {
	class Bitmap;
	class Buffer;
	class Image;
	class Sample;
}

class GameWindow;

// Types of Gosu resources we manage
typedef boost::scoped_ptr<Gosu::Buffer> BufferPtr;
typedef boost::shared_ptr<Gosu::Image> ImageRef;
typedef boost::shared_ptr<Gosu::Sample> SampleRef;
// libxml2 resources
typedef boost::shared_ptr<xmlDoc> XMLDocRef;

//! Resourcer Class
/*!
	This class provides the engine's global resource handling and caching.
*/
class Resourcer
{
public:
	//! Resourcer Constructor
	Resourcer(GameWindow* window, ClientValues* conf);

	//! Resourcer Destructor
	~Resourcer();

	//! Resourcer Initializer
	bool init();

	//! Requests an image resource from cache.
	ImageRef getImage(const std::string& name);

	//! Requests a bitmap that can be used to construct subimages from cache.
	void getBitmap(Gosu::Bitmap& bitmap, const std::string& name);

	//! Converts a subrectangle of a Bitmap into an Image.
	Gosu::Image* bitmapSection(const Gosu::Bitmap& src,
	        unsigned x, unsigned y, unsigned w, unsigned h, bool tileable);

	//! Requests an XML resource from cache.
	XMLDocRef getXMLDoc(const std::string& name);
	
	//! Returns a music stream from disk or cache.
	SampleRef getSample(const std::string& name);

private:
	typedef boost::unordered_map<std::string, ImageRef> ImageRefMap;
	typedef boost::unordered_map<std::string, SampleRef> SampleRefMap;
	typedef boost::unordered_map<std::string, XMLDocRef> XMLMap;
	typedef boost::unordered_map<std::string, std::string> StringMap;

	//! Requests an XML document from disk.
	xmlDoc* getXMLDocFromDisk(const std::string& name);

	//! Requests a string resource from cache.
	std::string getString(const std::string& name);
	std::string getStringFromDisk(const std::string& name);

	//! Read a generic resource from disk into memory.
	Gosu::Buffer* read(const std::string& name);

	//! Helper function 
	std::string path(const std::string& entry_name) const;

	GameWindow* window;
	zip* z;
	ClientValues* conf;

	//! Resource Cache
	/*!
		The key is the cached file's name, and the value is a pair of a
		"tally" and a pointer to the data. The "tally" is increased each
		time something requests the resource, and decreased each time
		something is finished with that resource. When the tally reaches
		zero, nothing is using the resource, and it is dropped after a
		few minutes. The cache drop timer is in a thread.
	*/
	ImageRefMap images;
	SampleRefMap samples;
	XMLMap xmls;
	StringMap strings;
};

#endif

