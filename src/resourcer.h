/******************************
** Tsunagari Tile Engine     **
** resourcer.h               **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef RESOURCER_H
#define RESOURCER_H

#include <deque>
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

// We hand out and manage Gosu resources in these forms:
typedef boost::scoped_ptr<Gosu::Buffer> BufferPtr;
typedef boost::shared_ptr<Gosu::Image> ImageRef;
typedef boost::shared_ptr<Gosu::Sample> SampleRef;
typedef std::deque<ImageRef> TiledImage;
// libxml2 resources
typedef boost::shared_ptr<xmlDoc> XMLDocRef;

//! Resourcer Class
/*!
	This class provides the engine's global resource handling and caching.
*/
class Resourcer
{
public:
	Resourcer(GameWindow* window, ClientValues* conf);
	~Resourcer();
	bool init();

	//! Expunge old stuff from the cache.
	void garbageCollect();

	//! Requests an image resource from cache.
	ImageRef getImage(const std::string& name);

	//! Requests an image resource from cache and splits it into a number
	//  of tiles each with width and height w by x. Returns false if the 
	//  source image wasn't found.
	bool getTiledImage(TiledImage& img, const std::string& name,
		unsigned w, unsigned h, bool tileable);

	//! Returns a music stream from disk or cache.
	SampleRef getSample(const std::string& name);

	//! Requests an XML resource from cache.
	XMLDocRef getXMLDoc(const std::string& name,
		const std::string& dtdPath);

private:
	template<class Res>
	struct CachedItem
	{
		Res resource;
		int lastUsed;
		int memoryUsed;
	};

	//! Resource maps.
	typedef boost::unordered_map<std::string, CachedItem<ImageRef> >
		ImageRefMap;
	typedef boost::unordered_map<std::string, CachedItem<SampleRef> >
		SampleRefMap;
	typedef boost::unordered_map<std::string, CachedItem<XMLDocRef> >
		XMLMap;
	typedef boost::unordered_map<std::string, TiledImage> TiledImageMap;

	//! Garbage collect a map.
	template<class Map, class MapValue>
	void reclaim(Map& map);

	//! Requests an XML document from disk.
	xmlDoc* readXMLDocFromDisk(const std::string& name,
		const std::string& dtdPath);

	//! Requests a string resource from disk.
	std::string readStringFromDisk(const std::string& name);

	//! Read a generic resource from disk.
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
	TiledImageMap tiles;
};

#endif

