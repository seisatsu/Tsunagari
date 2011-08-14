/******************************
** Tsunagari Tile Engine     **
** resourcer.h               **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef RESOURCER_H
#define RESOURCER_H

#include <deque>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <libxml/parser.h>

#include "common.h"
#include "lua.hpp"

struct zip;

namespace Gosu {
	class Bitmap;
	class Buffer;
	class Image;
	class Sample;
}

class GameWindow;

// We hand out and manage Gosu resources in these forms:
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

	//! Returns true if the world contains a resource by that name.
	bool resourceExists(const std::string& name);

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
		const std::string& dtdFile);

	//! Requests a Lua script from cache. Lua state L will parse the script.
	bool getLuaScript(const std::string& name, lua_State* L);

private:
	template<class Res>
	struct CacheEntry
	{
		Res resource;
		int lastUsed;
		int memoryUsed;
	};

	//! Resource maps.
	typedef boost::unordered_map<const std::string, CacheEntry<ImageRef> >
		ImageRefMap;
	typedef boost::unordered_map<const std::string, CacheEntry<
		boost::shared_ptr<TiledImage> > > TiledImageMap;
	typedef boost::unordered_map<const std::string, CacheEntry<SampleRef> >
		SampleRefMap;
	typedef boost::unordered_map<const std::string, CacheEntry<XMLDocRef> >
		XMLMap;

	// Holds compiled Lua scripts. Not garbage collected.
	typedef boost::unordered_map<const std::string, std::vector<char> >
		LuaBytecodeMap;


	//! Garbage collect a map.
	template<class Map, class MapValue>
	void reclaim(Map& map);

	//! Reads a Lua script from disk and parses it, returning the bytecode.
	bool compileLuaFromDisk(const std::string& name, lua_State* L,
                                std::vector<char>& bytes);

	//! Reads an XML document from disk and parses it.
	xmlDoc* readXMLDocFromDisk(const std::string& name,
		const std::string& dtdFile);

	//! Read a string resource from disk.
	std::string readStringFromDisk(const std::string& name);

	//! Read a generic resource from disk.
	Gosu::Buffer* read(const std::string& name);

	//! Helper function 
	std::string path(const std::string& entryName) const;

	GameWindow* window;
	zip* z;
	ClientValues* conf;

	// Cached resources stored in a manner usable by the game; no further
	// processing needed. Garbage collected.
	ImageRefMap images;
	TiledImageMap tiles;
	SampleRefMap samples;
	XMLMap xmls;

	// Not garbage collected.
	LuaBytecodeMap code;
};

#endif

