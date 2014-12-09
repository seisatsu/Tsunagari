/***************************************
** Tsunagari Tile Engine              **
** reader.cpp                         **
** Copyright 2011-2013 PariahSoft LLC **
***************************************/

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// **********

#include <errno.h>
#include <stdlib.h>

#include <Gosu/Bitmap.hpp>
#include <Gosu/Image.hpp>
#include <Gosu/IO.hpp>
#include <map>
#include <physfs.h>

#include "cache-template.cpp"
#include "client-conf.h"
#include "formatter.h"
#include "log.h"
#include "python.h"
#include "python-bindings-template.cpp"
#include "reader.h"
#include "script.h"
#include "window.h"
#include "xml.h"

#define ASSERT(x)  if (!(x)) { return false; }

typedef std::shared_ptr<xmlDtd> DTDRef;
typedef std::shared_ptr<std::string> StringRef;


// Caches that store processed, game-ready objects. Garbage collected.
Cache<ImageRef> images;
Cache<TiledImageRef> tiles;
Cache<SampleRef> sounds;
Cache<XMLRef> xmls;
Cache<StringRef> texts;

// DTDs don't expire. No garbage collection.
typedef std::map<std::string, DTDRef> DTDMap;
DTDMap dtds;



static std::string path(const std::string& entryName)
{
	// XXX: archive might not be world
	return conf.worldFilename + "/" + entryName;
}

template <class T>
static bool readFromDisk(const std::string& name, T& buf)
{
	PHYSFS_sint64 size;
	PHYSFS_File* zf;

	if (!PHYSFS_exists(name.c_str())) {
		Log::err("Reader", Formatter("%: file missing")
				% path(name));
		return false;
	}

	zf = PHYSFS_openRead(name.c_str());
	if (!zf) {
		Log::err("Reader", Formatter("%: error opening file: %")
				% path(name) % PHYSFS_getLastError());
		return false;
	}

	size = PHYSFS_fileLength(zf);
	if (size == -1) {
		Log::err("Reader", Formatter("%: could not determine file size: %")
				% path(name) % PHYSFS_getLastError());
		PHYSFS_close(zf);
		return false;
	}
	if (size > std::numeric_limits<uint32_t>::max()) {
		// FIXME: Technically, we just need to issue multiple calls to
		// PHYSFS_read. Fix when needed.
		Log::err("Reader", Formatter("%: file too long (>4GB)")
				% path(name));
		PHYSFS_close(zf);
		return false;
	}

	buf.resize(size);
	if (size == 0) {
		PHYSFS_close(zf);
		return true;
	}

	if (PHYSFS_read(zf, (char*)(buf.data()),
			(PHYSFS_uint32)size, 1) != 1) {
		Log::err("Reader", Formatter("%: error reading file: %")
				% path(name) % PHYSFS_getLastError());
		PHYSFS_close(zf);
		return false;
	}

	PHYSFS_close(zf);
	return true;
}

// FIXME: Should be moved to xml.cpp!!!!!!
static DTDRef parseDTD(const std::string& path)
{
	xmlCharEncoding enc = XML_CHAR_ENCODING_NONE;

	std::string bytes = Reader::readString(path);
	if (bytes.empty())
		return DTDRef();

	xmlParserInputBuffer* input = xmlParserInputBufferCreateMem(
			bytes.c_str(), (int)bytes.size(), enc);
	if (!input)
		return DTDRef();

	xmlDtd* dtd = xmlIOParseDTD(NULL, input, enc);
	if (!dtd)
		return DTDRef();

	return DTDRef(dtd, xmlFreeDtd);
}

// FIXME: Should be moved to xml.cpp!!!!!!
static bool preloadDTDs()
{
	ASSERT(dtds["dtd/area.dtd"]   = parseDTD("dtd/area.dtd"));
	ASSERT(dtds["dtd/entity.dtd"] = parseDTD("dtd/entity.dtd"));
	ASSERT(dtds["dtd/tsx.dtd"] = parseDTD("dtd/tsx.dtd"));
	ASSERT(dtds["dtd/world.dtd"]  = parseDTD("dtd/world.dtd"));
	return true;
}

static xmlDtd* getDTD(const std::string& name)
{
	DTDMap::iterator it = dtds.find(name);
	return it == dtds.end() ? NULL : it->second.get();
}

static XMLDoc* readXMLDoc(const std::string& name,
                          const std::string& dtdPath)
{
	std::string p = path(name);
	std::string data = Reader::readString(name);
	xmlDtd* dtd = getDTD(dtdPath);

	if (!dtd || data.empty())
		return NULL;
	XMLDoc* doc = new XMLDoc;
	if (!doc->init(p, data, dtd)) {
		delete doc;
		return NULL;
	}
	return doc;
}

static bool callInitpy(const std::string& archivePath)
{
	ASSERT(Reader::prependPath(archivePath));
	bool exists = Reader::resourceExists("__init__.py");
	Log::info(archivePath,
		std::string("__init__.py: ") +
		(exists ? "found" : "not found"));
	if (exists)
		// FIXME: Python will cache the __init__ module with no path prefix
		ASSERT(Script::create("__init__"));
	ASSERT(Reader::rmPath(archivePath));
	return true;
}











bool Reader::init(char* argv0)
{
	ASSERT(PHYSFS_init(argv0) != 0);

	// If any of our archives contain a file called "__init__.py", call it.
	for (Conf::StringVector::const_iterator it = conf.dataPath.begin(); it != conf.dataPath.end(); it++) {
		const std::string archive = *it;
		ASSERT(callInitpy(archive));
	}
	ASSERT(callInitpy(BASE_ZIP_PATH));

	ASSERT(prependPath(BASE_ZIP_PATH));

	// DTDs must be loaded from BASE_ZIP. They cannot be allowed to be
	// loaded from the world.
	ASSERT(preloadDTDs());

	ASSERT(prependPath(conf.worldFilename));
	for (Conf::StringVector::const_iterator it = conf.dataPath.begin(); it != conf.dataPath.end(); it++) {
		const std::string archive = *it;
		ASSERT(prependPath(archive));
	}

	return true;
}

void Reader::deinit()
{
	PHYSFS_deinit();
}

bool Reader::prependPath(const std::string& path)
{
	int err = PHYSFS_mount(path.c_str(), NULL, 0);
	if (err == 0) {
		Log::fatal("Reader", Formatter("%: could not open archive: %")
				% path % PHYSFS_getLastError());
		return false;
	}

	pythonPrependPath(path);

	return true;
}

bool Reader::appendPath(const std::string& path)
{
	int err = PHYSFS_mount(path.c_str(), NULL, 1);
	if (err == 0) {
		Log::fatal("Reader", Formatter("%: could not open archive: %")
				% path % PHYSFS_getLastError());
		return false;
	}

	pythonRmPath(path);

	return true;
}

bool Reader::rmPath(const std::string& path)
{
	int err = PHYSFS_removeFromSearchPath(path.c_str());
	if (err == 0) {
		Log::err("Reader", Formatter("libphysfs: %: %")
				% path % PHYSFS_getLastError());
		return false;
	}

	return true;
}

bool Reader::resourceExists(const std::string& name)
{
	return PHYSFS_exists(name.c_str());
}

bool Reader::directoryExists(const std::string& name)
{
	return resourceExists(name) && PHYSFS_isDirectory(name.c_str());
}

bool Reader::fileExists(const std::string& name)
{
	return resourceExists(name) && !PHYSFS_isDirectory(name.c_str());
}

Gosu::Buffer* Reader::readBuffer(const std::string& name)
{
	Gosu::Buffer* buf = new Gosu::Buffer();

	if (readFromDisk(name, *buf)) {
		return buf;
	}
	else {
		delete buf;
		return NULL;
	}
}

std::string Reader::readString(const std::string& name)
{
	std::string str;
	return readFromDisk(name, str) ? str : "";
}

ImageRef Reader::getImage(const std::string& name)
{
	ImageRef existing = images.lifetimeRequest(name);
	if (existing)
		return existing;

	std::unique_ptr<Gosu::Buffer> buffer(readBuffer(name));
	if (!buffer)
		return ImageRef();

	ImageRef result(Image::create(buffer->data(), buffer->size()));
	if (!result)
		return ImageRef();

	images.lifetimePut(name, result);
	return result;
}

TiledImageRef Reader::getTiledImage(const std::string& name,
		int w, int h)
{
	TiledImageRef existing = tiles.momentaryRequest(name);
	if (existing)
		return existing;

	std::unique_ptr<Gosu::Buffer> buffer(readBuffer(name));
	if (!buffer)
		return TiledImageRef();

	TiledImageRef result(
		TiledImage::create(buffer->data(), buffer->size(), w, h)
	);
	if (!result)
		return TiledImageRef();

	tiles.momentaryPut(name, result);
	return result;
}

SampleRef Reader::getSample(const std::string& name)
{
	if (!conf.audioEnabled)
		return SampleRef();

	SampleRef existing = sounds.lifetimeRequest(name);
	if (existing)
		return existing;

	std::unique_ptr<Gosu::Buffer> buffer(readBuffer(name));
	if (!buffer)
		return SampleRef();
	SampleRef result(new Sound(new Gosu::Sample(buffer->frontReader())));

	sounds.lifetimePut(name, result);
	return result;
}

XMLRef Reader::getXMLDoc(const std::string& name,
                            const std::string& dtdFile)
{
	XMLRef existing = xmls.momentaryRequest(name);
	if (existing)
		return existing;

	XMLRef result(readXMLDoc(name, dtdFile));

	xmls.momentaryPut(name, result);
	return result;
}

std::string Reader::getText(const std::string& name)
{
	StringRef existing = texts.momentaryRequest(name);
	if (existing)
		return *existing.get();

	StringRef result(new std::string(readString(name)));

	texts.momentaryPut(name, result);
	return *result.get();
}

void Reader::garbageCollect()
{
	images.garbageCollect();
	tiles.garbageCollect();
	sounds.garbageCollect();
	// songs.garbageCollect();
	xmls.garbageCollect();
	texts.garbageCollect();
}

void exportReader()
{
	// FIXME: Broken with shift to singleton. No instantiated object to bind.
	// Fix will require a stub object.

#if 0
	using namespace boost::python;

	class_<Reader>("Reader", no_init)
		.def("resource_exists", &Reader::resourceExists)
		.def("run_python_script", &Reader::runPythonScript)
		.def("get_text", &Reader::getText);
	pythonSetGlobal("Reader");
#endif
}

