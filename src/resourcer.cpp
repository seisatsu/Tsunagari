/*********************************
** Tsunagari Tile Engine        **
** resourcer.cpp                **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <errno.h>
#include <stdlib.h>

#include <boost/foreach.hpp>
#include <boost/python.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <Gosu/Bitmap.hpp>
#include <Gosu/Image.hpp>
#include <Gosu/IO.hpp>
#include <physfs.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "python.h"
#include "resourcer.h"
#include "window.h"
#include "xml.h"

typedef boost::scoped_ptr<Gosu::Buffer> BufferPtr;


Resourcer::Resourcer(GameWindow* window)
	: window(window)
{
	pythonSetResourcer(this);
	pythonSetGlobal("resourcer", this);
}

Resourcer::~Resourcer()
{
	PHYSFS_deinit();
}

bool Resourcer::init(char** argv)
{
	int err;
	err = PHYSFS_init(argv[0]);
	if (!err)
		return false;

	err = PHYSFS_mount(conf.world.c_str(), NULL, 0);
	if (!err) {
		Log::err("Resourcer", conf.world + ": could not open world");
		return false;
	}

	return true;
}

bool Resourcer::resourceExists(const std::string& name) const
{
	return PHYSFS_exists(name.c_str());
}

ImageRef Resourcer::getImage(const std::string& name)
{
	if (conf.cacheEnabled) {
		ImageRefMap::iterator entry = images.find(name);
		if (entry != images.end()) {
			if (entry->second.lastUsed) {
				Log::dbg("Resourcer", name + ": requested (cached)");
				entry->second.lastUsed = 0;
			}
			return entry->second.resource;
		}
	}
	Log::dbg("Resourcer", name + ": requested");

	BufferPtr buffer(read(name));
	if (!buffer)
		return ImageRef();
	Gosu::Bitmap bitmap;
	Gosu::loadImageFile(bitmap, buffer->frontReader());
	ImageRef result(new Gosu::Image(window->graphics(), bitmap, false));

	if (conf.cacheEnabled) {
		CacheEntry<ImageRef> data;
		data.resource = result;
		data.lastUsed = 0;
		images[name] = data;
	}

	return result;
}

bool Resourcer::getTiledImage(TiledImage& img, const std::string& name,
		int w, int h, bool tileable)
{
	if (conf.cacheEnabled) {
		TiledImageMap::iterator entry = tiles.find(name);
		if (entry != tiles.end()) {
			int now = GameWindow::getWindow().time();
			Log::dbg("Resourcer", name + ": requested (cached)");
			// We set lastUsed to now because it won't be used by
			// the time reclaim() gets to it.
			entry->second.lastUsed = now;
			img = *entry->second.resource.get();
			return true;
		}
	}
	Log::dbg("Resourcer", name + ": requested");

	BufferPtr buffer(read(name));
	if (!buffer)
		return false;
	Gosu::Bitmap bitmap;
	Gosu::loadImageFile(bitmap, buffer->frontReader());
	boost::shared_ptr<TiledImage> result(new TiledImage);
	Gosu::imagesFromTiledBitmap(window->graphics(), bitmap,
			(unsigned)w, (unsigned)h, tileable, *result.get());
	img = *result.get();

	if (conf.cacheEnabled) {
		CacheEntry<boost::shared_ptr<TiledImage> > data;
		data.resource = result;
		data.lastUsed = 0;
		tiles[name] = data;
	}

	return true;
}

SampleRef Resourcer::getSample(const std::string& name)
{
	if (!conf.audioEnabled)
		return SampleRef();

	if (conf.cacheEnabled) {
		SampleRefMap::iterator entry = samples.find(name);
		if (entry != samples.end()) {
			if (entry->second.lastUsed) {
				Log::dbg("Resourcer", name + ": requested (cached)");
				entry->second.lastUsed = 0;
			}
			return entry->second.resource;
		}
	}
	Log::dbg("Resourcer", name + ": requested");

	BufferPtr buffer(read(name));
	if (!buffer)
		return SampleRef();
	SampleRef result(new Sample(new Gosu::Sample(buffer->frontReader())));

	if (conf.cacheEnabled) {
		CacheEntry<SampleRef> data;
		data.resource = result;
		data.lastUsed = 0;
		samples[name] = data;
	}

	return result;
}

SongRef Resourcer::getSong(const std::string& name)
{
	if (!conf.audioEnabled)
		return SongRef();

	if (conf.cacheEnabled) {
		SongRefMap::iterator entry = songs.find(name);
		if (entry != songs.end()) {
			if (entry->second.lastUsed) {
				Log::dbg("Resourcer", name + ": requested (cached)");
				entry->second.lastUsed = 0;
			}
			return entry->second.resource;
		}
	}
	Log::dbg("Resourcer", name + ": requested");

	BufferPtr buffer(read(name));
	if (!buffer)
		return SongRef();
	SongRef result(new Gosu::Song(buffer->frontReader()));

	if (conf.cacheEnabled) {
		CacheEntry<SongRef> data;
		data.resource = result;
		data.lastUsed = 0;
		songs[name] = data;
	}

	return result;
}

XMLRef Resourcer::getXMLDoc(const std::string& name,
                            const std::string& dtdFile)
{
	if (conf.cacheEnabled) {
		XMLRefMap::iterator entry = xmls.find(name);
		if (entry != xmls.end()) {
			int now = GameWindow::getWindow().time();
			Log::dbg("Resourcer", name + ": requested (cached)");
			// We set lastUsed to now because it won't be used by
			// the time reclaim() gets to it.
			entry->second.lastUsed = now;
			return entry->second.resource;
		}
	}
	Log::dbg("Resourcer", name + ": requested");

	XMLRef result(readXMLDocFromDisk(name, dtdFile));

	if (conf.cacheEnabled) {
		CacheEntry<XMLRef> data;
		data.resource = result;
		data.lastUsed = 0;
		xmls[name] = data;
	}

	return result;
}

bool Resourcer::runPythonScript(const std::string& name)
{
	if (conf.cacheEnabled) {
		CodeMap::iterator entry = codes.find(name);
		if (entry != codes.end()) {
			int now = GameWindow::getWindow().time();
			Log::dbg("Resourcer", name + ": requested (cached)");
			// We set lastUsed to now because it won't be used by
			// the time reclaim() gets to it.
			entry->second.lastUsed = now;
			PyCodeObject* result = entry->second.resource;
			return pythonExec(result);
		}
	}
	Log::dbg("Resourcer", name + ": requested");

	std::string code = readStringFromDisk(name);
	PyCodeObject* result = code.size() ?
		pythonCompile(name.c_str(), code.c_str()) : NULL;

	if (conf.cacheEnabled) {
		CacheEntry<PyCodeObject*> data;
		data.resource = result;
		data.lastUsed = 0;
		codes[name] = data;
	}

	return pythonExec(result);
}

std::string Resourcer::getText(const std::string& name)
{
	if (conf.cacheEnabled) {
		TextRefMap::iterator entry = texts.find(name);
		if (entry != texts.end()) {
			int now = GameWindow::getWindow().time();
			Log::dbg("Resourcer", name + ": requested (cached)");
			// We set lastUsed to now because it won't be used by
			// the time reclaim() gets to it.
			entry->second.lastUsed = now;
			return *entry->second.resource.get();
		}
	}
	Log::dbg("Resourcer", name + ": requested");

	StringRef result(new std::string(readStringFromDisk(name)));

	if (conf.cacheEnabled) {
		CacheEntry<StringRef> data;
		data.resource = result;
		data.lastUsed = 0;
		texts[name] = data;
	}

	return *result.get();
}

void Resourcer::garbageCollect()
{
	reclaim<ImageRefMap, ImageRef>(images);
	reclaim<TiledImageMap, boost::shared_ptr<TiledImage> >(tiles);
	reclaim<SampleRefMap, SampleRef>(samples);
	reclaim<SongRefMap, SongRef>(songs);
	reclaim<XMLRefMap, XMLRef>(xmls);
	reclaim<TextRefMap, StringRef>(texts);
}

template<class Map, class MapValue>
void Resourcer::reclaim(Map& map)
{
	int now = GameWindow::getWindow().time();
	std::vector<std::string> dead;
	BOOST_FOREACH(typename Map::value_type& i, map) {
		const std::string& name = i.first;
		CacheEntry<MapValue>& cache = i.second;
		bool unused = !cache.resource || cache.resource.unique();
		if (unused) {
			if (!cache.lastUsed) {
				cache.lastUsed = now;
				//Log::dbg("Resourcer", name + ": unused");
			}
			else if (now < cache.lastUsed) {
				// Handle time overflow
				cache.lastUsed = now;
			}
			else if (now > cache.lastUsed + conf.cacheTTL*1000) {
				dead.push_back(name);
				Log::dbg("Resourcer", name + ": purged from cache");
			}
		}
		// XXX: Redundant? We're working around this because it won't
		// catch XML documents.
		else if (cache.lastUsed) {
			cache.lastUsed = 0;
			Log::dbg("Resourcer", name + ": requested (cached)");
		}
	}
	BOOST_FOREACH(std::string& name, dead)
		map.erase(name);
}

XMLDoc* Resourcer::readXMLDocFromDisk(const std::string& name,
                                     const std::string& dtdFile) const
{
	XMLDoc* doc = new XMLDoc;
	const std::string data = readStringFromDisk(name);
	if (data.size()) {
		const std::string p = path(name);
		const std::string dtdPath = std::string(DTD_DIRECTORY) +
			"/" + dtdFile;
		doc->init(p, data, dtdPath); // Ignore return value?
	}
	return doc;
}

std::string Resourcer::readStringFromDisk(const std::string& name) const
{
	unsigned long size;
	boost::scoped_array<char> buf;
	std::string str;
	PHYSFS_File* zf;

	if (!PHYSFS_exists(name.c_str())) {
		Log::err("Resourcer", path(name) + ": file missing");
		return "";
	}

	zf = PHYSFS_openRead(name.c_str());
	if (!zf) {
		Log::err("Resourcer", path(name) + ": error opening file");
		return "";
	}

	size = (unsigned long)PHYSFS_fileLength(zf);
	buf.reset(new char[size + 1]);
	buf[size] = '\0';

	if (PHYSFS_read(zf, buf.get(), 1,
	   (PHYSFS_uint32)PHYSFS_fileLength(zf)) == -1) {
		Log::err("Resourcer", path(name) + ": general I/O error"
			" during loading");
		PHYSFS_close(zf);
		return "";
	}

	str = buf.get();

	PHYSFS_close(zf);
	return str;
}

Gosu::Buffer* Resourcer::read(const std::string& name) const
{
	unsigned long size;
	PHYSFS_File* zf;

	if (!PHYSFS_exists(name.c_str())) {
		Log::err("Resourcer", path(name) + ": file missing");
		return NULL;
	}

	zf = PHYSFS_openRead(name.c_str());
	if (!zf) {
		Log::err("Resourcer", path(name) + ": error opening file");
		return NULL;
	}

	size = (unsigned long)PHYSFS_fileLength(zf);

	Gosu::Buffer* buffer = new Gosu::Buffer;
	buffer->resize(size);
	if (PHYSFS_read(zf, buffer->data(), 1,
	   (PHYSFS_uint32)PHYSFS_fileLength(zf)) == -1) {
		Log::err("Resourcer", path(name) + ": general I/O error"
			" during loading");
		PHYSFS_close(zf);
		delete buffer;
		return NULL;
	}

	PHYSFS_close(zf);
	return buffer;
}

std::string Resourcer::path(const std::string& entryName) const
{
	return conf.world + "/" + entryName;
}



void exportResourcer()
{
	boost::python::class_<Resourcer>("Resourcer", boost::python::no_init)
		.def("resourceExists", &Resourcer::resourceExists)
		.def("runPythonScript", &Resourcer::runPythonScript)
		.def("getSound", &Resourcer::getSample)
		.def("getText", &Resourcer::getText);
}

