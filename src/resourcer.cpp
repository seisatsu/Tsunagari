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


static Resourcer* globalResourcer = NULL;

Resourcer* Resourcer::getResourcer()
{
	return globalResourcer;
}

Resourcer::Resourcer()
{
	globalResourcer = this;
	pythonSetGlobal("resourcer", this);
}

Resourcer::~Resourcer()
{
	PHYSFS_deinit();
}

bool Resourcer::init(char* argv0)
{
	int err;
	err = PHYSFS_init(argv0);
	if (!err)
		return false;

	err = PHYSFS_mount(conf.worldFilename.c_str(), NULL, 0);
	if (!err) {
		Log::fatal("Resourcer", conf.worldFilename + ": could not open world");
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
		GameWindow::getWindow().graphics(), bitmap, false));

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
	Gosu::imagesFromTiledBitmap(GameWindow::getWindow().graphics(), bitmap,
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
	return conf.worldFilename + "/" + entryName;
}



void exportResourcer()
{
	boost::python::class_<Resourcer>("Resourcer", boost::python::no_init)
		.def("resourceExists", &Resourcer::resourceExists)
		.def("runPythonScript", &Resourcer::runPythonScript)
		.def("getSound", &Resourcer::getSample)
		.def("getText", &Resourcer::getText);
}

