/******************************
** Tsunagari Tile Engine     **
** resourcer.cpp             **
** Copyright 2011 OmegaSDG   **
******************************/

#include <errno.h>
#include <stdlib.h>

#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <Gosu/Audio.hpp>
#include <Gosu/Bitmap.hpp>
#include <Gosu/Image.hpp>
#include <Gosu/IO.hpp>
#include <zip.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "resourcer.h"
#include "window.h"
#include "xml.h"

#ifndef LIBXML_TREE_ENABLED
	#error Tree must be enabled in libxml2
#endif

typedef boost::scoped_ptr<Gosu::Buffer> BufferPtr;

Resourcer::Resourcer(GameWindow* window, ClientValues* conf)
	: window(window), z(NULL), conf(conf)
{
}

Resourcer::~Resourcer()
{
	if (z && zip_close(z))
		Log::err(conf->world,
		         std::string("closing : ") + zip_strerror(z));
}

bool Resourcer::init()
{
	int err;
	z = zip_open(conf->world.c_str(), 0x0, &err);
	if (!z) {
		char buf[512];
		zip_error_to_str(buf, sizeof(buf), err, errno);
		Log::err(conf->world, buf);
	}

	return z;
}

void Resourcer::garbageCollect()
{
	reclaim<ImageRefMap, ImageRef>(images);
	reclaim<TiledImageMap, boost::shared_ptr<TiledImage> >(tiles);
	reclaim<SampleRefMap, SampleRef>(samples);
	reclaim<XMLMap, XMLDocRef>(xmls);
}

template<class Map, class MapValue>
void Resourcer::reclaim(Map& map)
{
	int now = GameWindow::getWindow().time();
	std::vector<std::string> dead;
	BOOST_FOREACH(typename Map::value_type& i, map) {
		const std::string& name = i.first;
		CacheEntry<MapValue>& cache = i.second;
		long extUses = cache.resource.use_count() - 1;
		if (extUses == 0) {
			if (!cache.lastUsed) {
				cache.lastUsed = now;
				Log::dbg("Resourcer", name + " unused");
			}
			else if (now < cache.lastUsed) {
				// Handle time overflow
				cache.lastUsed = now;
			}
			else if (now > cache.lastUsed + conf->cacheTTL*1000) {
				dead.push_back(name);
				Log::dbg("Resourcer", "Removing " + name);
			}
		}
		// XXX: Redundant? We're working around this because it won't
		// catch XML documents.
		else if (cache.lastUsed) {
			cache.lastUsed = 0;
			Log::dbg("Resourcer", name + " used again");
		}
	}
	BOOST_FOREACH(std::string& name, dead)
		map.erase(name);
}

bool Resourcer::resourceExists(const std::string& name)
{
	struct zip_stat stat;
	return zip_stat(z, name.c_str(), 0x0, &stat) == 0;
}

ImageRef Resourcer::getImage(const std::string& name)
{
	if (conf->cacheEnabled) {
		ImageRefMap::iterator entry = images.find(name);
		if (entry != images.end()) {
			if (entry->second.lastUsed) {
				Log::dbg("Resourcer", name + " used again");
				entry->second.lastUsed = 0;
			}
			return entry->second.resource;
		}
	}

	BufferPtr buffer(read(name));
	if (!buffer)
		return ImageRef();
	Gosu::Bitmap bitmap;
	Gosu::loadImageFile(bitmap, buffer->frontReader());
	ImageRef result(new Gosu::Image(window->graphics(), bitmap, false));

	if (conf->cacheEnabled) {
		CacheEntry<ImageRef> data;
		data.resource = result;
		data.lastUsed = 0; 
		images[name] = data;
	}
	return result;
}

bool Resourcer::getTiledImage(TiledImage& img, const std::string& name,
		unsigned w, unsigned h, bool tileable)
{
	if (conf->cacheEnabled) {
		TiledImageMap::iterator entry = tiles.find(name);
		if (entry != tiles.end()) {
			int now = GameWindow::getWindow().time();
			Log::dbg("Resourcer", name + " used again");
			// We set lastUsed to now because it won't be used by
			// the time reclaim() gets to it.
			entry->second.lastUsed = now;
			img = *entry->second.resource.get();
			return true;
		}
	}

	BufferPtr buffer(read(name));
	if (!buffer)
		return false;
	Gosu::Bitmap bitmap;
	Gosu::loadImageFile(bitmap, buffer->frontReader());
	boost::shared_ptr<TiledImage> result(new TiledImage);
	Gosu::imagesFromTiledBitmap(window->graphics(), bitmap, w, h,
			tileable, *result.get());
	img = *result.get();

	if (conf->cacheEnabled) {
		CacheEntry<boost::shared_ptr<TiledImage> > data;
		data.resource = result;
		data.lastUsed = 0;
		tiles[name] = data;
	}
	return true;
}

/* FIXME
 * We use Gosu::Sample for music because Gosu::Song's SDL implementation
 * doesn't support loading from a memory buffer at the moment.
 */
SampleRef Resourcer::getSample(const std::string& name)
{
	if (conf->cacheEnabled) {
		SampleRefMap::iterator entry = samples.find(name);
		if (entry != samples.end()) {
			if (entry->second.lastUsed) {
				Log::dbg("Resourcer", name + " used again");
				entry->second.lastUsed = 0;
			}
			return entry->second.resource;
		}
	}

	BufferPtr buffer(read(name));
	if (!buffer)
		return SampleRef();
	SampleRef result(new Gosu::Sample(buffer->frontReader()));

	if (conf->cacheEnabled) {
		CacheEntry<SampleRef> data;
		data.resource = result;
		data.lastUsed = 0; 
		samples[name] = data;
	}
	return result;
}

XMLDocRef Resourcer::getXMLDoc(const std::string& name,
                               const std::string& dtdFile)
{
	if (conf->cacheEnabled) {
		XMLMap::iterator entry = xmls.find(name);
		if (entry != xmls.end()) {
			int now = GameWindow::getWindow().time();
			Log::dbg("Resourcer", name + " used again");
			// We set lastUsed to now because it won't be used by
			// the time reclaim() gets to it.
			entry->second.lastUsed = now;
			return entry->second.resource;
		}
	}

	XMLDocRef result(readXMLDocFromDisk(name, dtdFile), xmlFreeDoc);
	// XXX Do we check for NULL?

	if (conf->cacheEnabled) {
		CacheEntry<XMLDocRef> data;
		data.resource = result;
		data.lastUsed = 0; 
		xmls[name] = data;
	}
	return result;
}

//! Take a Lua compiler error message and format it like one of our error
//! messages.
static void luaParseError(const std::string& name, lua_State* L)
{
	const char* err = lua_tostring(L, -1);
	const char* afterFile = strchr(err, ':') + 1; // +1 for ':'
	const char* afterLine = strchr(afterFile, ':') + 2; // +2 for ': '
	char line[512];
	memcpy(line, afterFile, afterLine - afterFile - 2);
	Log::err(name + ":" + line, std::string("parsing error: ") + afterLine);
}

template <class Container>
static bool runLuaScript(lua_State* L, const Container& c, const char* name)
{
	int status = luaL_loadbuffer(L, c.data(), c.size(), name);

	switch (status) {
	case LUA_ERRSYNTAX:
		luaParseError(name, L);
		return false;
	case LUA_ERRMEM:
		// Should we even bother with this?
		Log::err(name, "out of memory while parsing");
		return false;
	}

	return true;
}

bool Resourcer::getLuaScript(const std::string& name, lua_State* L)
{
	if (conf->cacheEnabled) {
		LuaBytecodeMap::iterator entry = code.find(name);
		if (entry != code.end())
			return runLuaScript(L, entry->second, name.c_str());
	}

	std::vector<char> bytecode;
	bool found = compileLuaFromDisk(name, L, bytecode);
	if (!found) // error already logged
		return false;

	if (conf->cacheEnabled)
		code[name].swap(bytecode);

	// lua_State* L was the object that compiled the script, so we don't
	// have to run the bytecode again.
	return true;
}

static int writeToVector(lua_State*, const void* data, size_t sz, void* vector)
{
	std::vector<char>* v = (std::vector<char>*)vector;
	char* s = (char*)data; // GCC: can't do ++ on a const void*
	while (sz--)
		v->push_back(*s++);
	return 0;
}

bool Resourcer::compileLuaFromDisk(const std::string& name, lua_State* L,
                                   std::vector<char>& bytes)
{
	const std::string script = readStringFromDisk(name);
	if (script.empty()) // error already logged
		return false;

	// Compile the Lua script.
	bool compiled = runLuaScript(L, script, name.c_str());
	if (!compiled) // error already logged
		return false;

	// Save the bytecode for the script to a vector.
	int error = lua_dump(L, writeToVector, &bytes);
	if (error)
		return false;

	return true;
}

// use RAII to ensure doc is freed
// boost::shared_ptr<void> alwaysFreeTheDoc(doc, xmlFreeDoc);
xmlDoc* Resourcer::readXMLDocFromDisk(const std::string& name,
                                      const std::string& dtdFile)
{
	const std::string docStr = readStringFromDisk(name);
	if (docStr.empty())
		return NULL;
	const std::string pathname = path(name);
	XMLDocument doc;
	std::string dtdPath = std::string(DTD_DIRECTORY) + "/" + dtdFile;
	if (doc.init(pathname, docStr, dtdPath))
		return doc.temporaryGetDoc();
	else
		return NULL;
}

std::string Resourcer::readStringFromDisk(const std::string& name)
{
	struct zip_stat stat;
	zip_file* zf;
	int size;
	char* buf;
	std::string str;

	if (zip_stat(z, name.c_str(), 0x0, &stat)) {
		Log::err(path(name), "file missing");
		return "";
	}

	size = (int)stat.size;
	buf = new char[size + 1];
	buf[size] = '\0';

	zf = zip_fopen(z, name.c_str(), 0x0);
	if (!zf) {
		Log::err(path(name),
		         std::string("opening : ") + zip_strerror(z));
		return "";
	}

	if (zip_fread(zf, buf, size) != size) {
		Log::err(path(name), "reading didn't complete");
		zip_fclose(zf);
		return "";
	}

	str = buf;
	delete[] buf;

	zip_fclose(zf);
	return str;
}

Gosu::Buffer* Resourcer::read(const std::string& name)
{
	struct zip_stat stat;
	zip_file* zf;
	int size;

	if (zip_stat(z, name.c_str(), 0x0, &stat)) {
		Log::err(path(name), "file missing");
		return NULL;
	}

	size = (int)stat.size;

	if (!(zf = zip_fopen(z, name.c_str(), 0x0))) {
		Log::err(path(name),
		         std::string("opening : ") + zip_strerror(z));
		return NULL;
	}

	Gosu::Buffer* buffer = new Gosu::Buffer;
	buffer->resize(size);
	if (zip_fread(zf, buffer->data(), size) != size) {
		Log::err(path(name), "reading didn't complete");
		zip_fclose(zf);
		delete buffer;
		return NULL;
	}

	zip_fclose(zf);
	return buffer;
}

std::string Resourcer::path(const std::string& entryName) const
{
	return conf->world + "/" + entryName;
}

