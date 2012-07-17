/*********************************
** Tsunagari Tile Engine        **
** resourcer.cpp                **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <errno.h>
#include <stdlib.h>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/python.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <Gosu/Bitmap.hpp>
#include <Gosu/Image.hpp>
#include <Gosu/IO.hpp>
#include <physfs.h>

#include "client-conf.h"
#include "log.h"
#include "python.h"
#include "resourcer.h"
#include "window.h"
#include "xml.h"

#define ASSERT(x)  if (!(x)) { return false; }

typedef boost::scoped_ptr<Gosu::Buffer> BufferPtr;


static Resourcer* globalResourcer = NULL;

Resourcer* Resourcer::instance()
{
	return globalResourcer;
}

Resourcer::Resourcer()
{
	globalResourcer = this;
	pythonSetGlobal("Resourcer", this);
}

Resourcer::~Resourcer()
{
	PHYSFS_deinit();
}

static bool callInitpy(Resourcer* rc, const std::string& archivePath)
{
	ASSERT(rc->prependPath(archivePath));
	bool exists = rc->resourceExists("init.py");
	Log::info(archivePath,
		std::string("init.py: ") +
		(exists ? "found" : "not found"));
	if (exists)
		ASSERT(rc->runPythonScript("init.py"));
	ASSERT(rc->rmPath(archivePath));
	return true;
}

bool Resourcer::init(char* argv0)
{
	ASSERT(PHYSFS_init(argv0) != 0);

	// If any of our archives contain a file called "init.py", call it.
	BOOST_FOREACH(std::string archivePath, conf.dataPath)
		ASSERT(callInitpy(this, archivePath));
	ASSERT(callInitpy(this, BASE_ZIP_PATH));

	BOOST_FOREACH(std::string pathname, conf.dataPath)
		ASSERT(prependPath(pathname));
	ASSERT(appendPath(conf.worldFilename));
	ASSERT(appendPath(BASE_ZIP_PATH));

	return true;
}

bool Resourcer::prependPath(const std::string& path)
{
	using namespace boost;

	int err = PHYSFS_mount(path.c_str(), NULL, 0);
	if (err == 0) {
		Log::fatal("Resourcer", str(
				format("%s: could not open archive: %s")
				% path % PHYSFS_getLastError()));
		return false;
	}

	return true;
}

bool Resourcer::appendPath(const std::string& path)
{
	using namespace boost;

	int err = PHYSFS_mount(path.c_str(), NULL, 1);
	if (err == 0) {
		Log::fatal("Resourcer", str(
				format("%s: could not open archive: %s")
				% path % PHYSFS_getLastError()));
		return false;
	}

	return true;
}

bool Resourcer::rmPath(const std::string& path)
{
	using namespace boost;

	int err = PHYSFS_removeFromSearchPath(path.c_str());
	if (err == 0) {
		Log::err("Resourcer", str(format("libphysfs: %s: %s")
				% path % PHYSFS_getLastError()));
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
	ImageRef existing = images.lifetimeRequest(name);
	if (existing)
		return existing;

	BufferPtr buffer(read(name));
	if (!buffer)
		return ImageRef();
	Gosu::Bitmap bitmap;
	Gosu::loadImageFile(bitmap, buffer->frontReader());
	ImageRef result(new Gosu::Image(
		GameWindow::instance().graphics(), bitmap, false));

	images.lifetimePut(name, result);
	return result;
}

bool Resourcer::getTiledImage(TiledImage& img, const std::string& name,
		int w, int h, bool tileable)
{
	TiledImageRef existing = tiles.momentaryRequest(name);
	if (existing) {
		img = *existing.get();
		return true;
	}

	BufferPtr buffer(read(name));
	if (!buffer)
		return false;
	Gosu::Bitmap bitmap;
	Gosu::loadImageFile(bitmap, buffer->frontReader());
	TiledImageRef result(new TiledImage);
	Gosu::imagesFromTiledBitmap(GameWindow::instance().graphics(), bitmap,
			(unsigned)w, (unsigned)h, tileable, *result.get());
	img = *result.get();

	tiles.momentaryPut(name, result);
	return true;
}

SampleRef Resourcer::getSample(const std::string& name)
{
	if (!conf.audioEnabled)
		return SampleRef();

	SampleRef existing = sounds.lifetimeRequest(name);
	if (existing)
		return existing;

	BufferPtr buffer(read(name));
	if (!buffer)
		return SampleRef();
	SampleRef result(new Sound(new Gosu::Sample(buffer->frontReader())));

	sounds.lifetimePut(name, result);
	return result;
}

SongRef Resourcer::getSong(const std::string& name)
{
	if (!conf.audioEnabled)
		return SongRef();

	SongRef existing = songs.lifetimeRequest(name);
	if (existing)
		return existing;

	BufferPtr buffer(read(name));
	if (!buffer)
		return SongRef();
	SongRef result(new Gosu::Song(buffer->frontReader()));

	songs.lifetimePut(name, result);
	return result;
}

XMLRef Resourcer::getXMLDoc(const std::string& name,
                            const std::string& dtdFile)
{
	XMLRef existing = xmls.momentaryRequest(name);
	if (existing)
		return existing;

	XMLRef result(readXMLDocFromDisk(name, dtdFile));

	xmls.momentaryPut(name, result);
	return result;
}

bool Resourcer::runPythonScript(const std::string& name)
{
	PyCodeObject* existing = codes.momentaryRequest(name);
	if (existing)
		return pythonExec(existing);

	std::string code = readStringFromDisk(name);
	PyCodeObject* result = code.size() ?
		pythonCompile(name.c_str(), code.c_str()) : NULL;

	codes.momentaryPut(name, result);
	return pythonExec(result);
}

std::string Resourcer::getText(const std::string& name)
{
	StringRef existing = texts.momentaryRequest(name);
	if (existing)
		return *existing.get();

	StringRef result(new std::string(readStringFromDisk(name)));

	texts.momentaryPut(name, result);
	return *result.get();
}

void Resourcer::garbageCollect()
{
	images.garbageCollect();
	tiles.garbageCollect();
	sounds.garbageCollect();
	songs.garbageCollect();
	xmls.garbageCollect();
	texts.garbageCollect();
}

template <class T>
bool Resourcer::readFromDisk(const std::string& name, T& buf)
{
	using namespace boost;

	PHYSFS_sint64 size;
	PHYSFS_File* zf;

	if (!PHYSFS_exists(name.c_str())) {
		Log::err("Resourcer", str(format("%s: file missing")
				% path(name)));
		return false;
	}

	zf = PHYSFS_openRead(name.c_str());
	if (!zf) {
		Log::err("Resourcer", str(format("%s: error opening file: %s")
				% path(name) % PHYSFS_getLastError()));
		return false;
	}

	size = PHYSFS_fileLength(zf);
	if (size == -1) {
		Log::err("Resourcer", str(
				format("%s: could not determine file size: %s")
				% path(name) % PHYSFS_getLastError()));
		PHYSFS_close(zf);
		return false;
	}
	if (size > std::numeric_limits<uint32_t>::max()) {
		// FIXME: Technically, we just need to issue multiple calls to
		// PHYSFS_read. Fix when needed.
		Log::err("Resourcer", str(format("%s: file too long (>4GB)")
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
		Log::err("Resourcer", str(format("%s: error reading file: %s")
				% path(name) % PHYSFS_getLastError()));
		PHYSFS_close(zf);
		return false;
	}

	PHYSFS_close(zf);
	return true;
}

XMLDoc* Resourcer::readXMLDocFromDisk(const std::string& name,
		const std::string& dtdFile)
{
	using namespace boost;

	XMLDoc* doc = new XMLDoc;
	std::string data = readStringFromDisk(name);
	if (data.size()) {
		std::string p = path(name);
		std::string dtdPath = str(format("%s/%s")
				% XML_DTD_PATH % dtdFile);
		if (!doc->init(p, data, dtdPath)) {
			delete doc;
			doc = NULL;
		}
	}
	return doc;
}

std::string Resourcer::readStringFromDisk(const std::string& name)
{
	std::string str;
	return readFromDisk(name, str) ? str : "";
}

Gosu::Buffer* Resourcer::read(const std::string& name)
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

std::string Resourcer::path(const std::string& entryName) const
{
	return conf.worldFilename + "/" + entryName;
}



void exportResourcer()
{
	using namespace boost::python;

	class_<Resourcer>("Resourcer", no_init)
		.def("resource_exists", &Resourcer::resourceExists)
		.def("run_python_script", &Resourcer::runPythonScript)
		.def("get_text", &Resourcer::getText);
}

