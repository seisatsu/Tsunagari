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

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/python.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <Gosu/Bitmap.hpp>
#include <Gosu/Image.hpp>
#include <Gosu/IO.hpp>
#include <physfs.h>

#include "bytecode.h"
#include "client-conf.h"
#include "log.h"
#include "python.h"
#include "reader.h"
#include "window.h"
#include "xml.h"

#define ASSERT(x)  if (!(x)) { return false; }

typedef boost::shared_ptr<xmlDtd> DTDRef;
typedef boost::shared_ptr<std::string> StringRef;


// Caches that store processed, game-ready objects. Garbage collected.
Cache<ImageRef> images;
Cache<TiledImageRef> tiles;
Cache<SampleRef> sounds;
Cache<XMLRef> xmls;
Cache<BytecodeRef> bytecodes;
Cache<StringRef> texts;

// DTDs don't expire. No garbage collection.
typedef boost::unordered_map<std::string, DTDRef> TextMap;
TextMap dtds;



static std::string path(const std::string& entryName)
{
	// XXX: archive might not be world
	return conf.worldFilename + "/" + entryName;
}

template <class T>
static bool readFromDisk(const std::string& name, T& buf)
{
	using namespace boost;

	PHYSFS_sint64 size;
	PHYSFS_File* zf;

	if (!PHYSFS_exists(name.c_str())) {
		Log::err("Reader", str(format("%s: file missing")
				% path(name)));
		return false;
	}

	zf = PHYSFS_openRead(name.c_str());
	if (!zf) {
		Log::err("Reader", str(format("%s: error opening file: %s")
				% path(name) % PHYSFS_getLastError()));
		return false;
	}

	size = PHYSFS_fileLength(zf);
	if (size == -1) {
		Log::err("Reader", str(
				format("%s: could not determine file size: %s")
				% path(name) % PHYSFS_getLastError()));
		PHYSFS_close(zf);
		return false;
	}
	if (size > std::numeric_limits<uint32_t>::max()) {
		// FIXME: Technically, we just need to issue multiple calls to
		// PHYSFS_read. Fix when needed.
		Log::err("Reader", str(format("%s: file too long (>4GB)")
				% path(name)));
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
		Log::err("Reader", str(format("%s: error reading file: %s")
				% path(name) % PHYSFS_getLastError()));
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
	TextMap::iterator it = dtds.find(name);
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
		ASSERT(Reader::runPythonScript("__init__.py"));
	ASSERT(Reader::rmPath(archivePath));
	return true;
}











bool Reader::init(char* argv0)
{
	ASSERT(PHYSFS_init(argv0) != 0);

	// If any of our archives contain a file called "init.py", call it.
	BOOST_FOREACH(std::string archive, conf.dataPath)
		ASSERT(callInitpy(archive));
	ASSERT(callInitpy(BASE_ZIP_PATH));

	ASSERT(prependPath(BASE_ZIP_PATH));

	// DTDs must be loaded from BASE_ZIP. They cannot be allowed to be
	// loaded from the world.
	ASSERT(preloadDTDs());

	ASSERT(prependPath(conf.worldFilename));
	BOOST_FOREACH(std::string pathname, conf.dataPath)
		ASSERT(prependPath(pathname));

	return true;
}

void Reader::deinit()
{
	PHYSFS_deinit();
}

bool Reader::prependPath(const std::string& path)
{
	using namespace boost;

	int err = PHYSFS_mount(path.c_str(), NULL, 0);
	if (err == 0) {
		Log::fatal("Reader", str(
				format("%s: could not open archive: %s")
				% path % PHYSFS_getLastError()));
		return false;
	}

	return true;
}

bool Reader::appendPath(const std::string& path)
{
	using namespace boost;

	int err = PHYSFS_mount(path.c_str(), NULL, 1);
	if (err == 0) {
		Log::fatal("Reader", str(
				format("%s: could not open archive: %s")
				% path % PHYSFS_getLastError()));
		return false;
	}

	return true;
}

bool Reader::rmPath(const std::string& path)
{
	using namespace boost;

	int err = PHYSFS_removeFromSearchPath(path.c_str());
	if (err == 0) {
		Log::err("Reader", str(format("libphysfs: %s: %s")
				% path % PHYSFS_getLastError()));
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

	boost::scoped_ptr<Gosu::Buffer> buffer(readBuffer(name));
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

	boost::scoped_ptr<Gosu::Buffer> buffer(readBuffer(name));
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

	boost::scoped_ptr<Gosu::Buffer> buffer(readBuffer(name));
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

BytecodeRef Reader::getBytecode(const std::string& path)
{
	BytecodeRef existing = bytecodes.momentaryRequest(path);
	if (existing)
		return existing;

	BytecodeRef result;
	if (fileExists(path)) {
		std::string code = readString(path);
		Bytecode* script = new Bytecode(path, code);
		result.reset(script);
	}

	bytecodes.momentaryPut(path, result);
	return result;
}

bool Reader::runPythonScript(const std::string& path)
{
	BytecodeRef bc = getBytecode(path);
	if (bc && bc->valid()) {
		bc->execute();
		return true;
	}
	else {
		Log::err("Script",
		    bc->filename() + ": Attempt to run broken Python script");
		return false;
	}
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

