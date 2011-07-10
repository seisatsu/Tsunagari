/******************************
** Tsunagari Tile Engine     **
** resourcer.cpp             **
** Copyright 2011 OmegaSDG   **
******************************/

#include <errno.h>
#include <stdlib.h>

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

static void xmlErrorCb(void* pstrFilename, const char* msg, ...)
{
	const std::string* filename = (const std::string*)pstrFilename;
	char buf[512];
	va_list ap;

	va_start(ap, msg);
	snprintf(buf, sizeof(buf)-1, msg, va_arg(ap, char*));
	Log::err(*filename, buf);
	va_end(ap);
}

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

ImageRef Resourcer::getImage(const std::string& name)
{
	if (conf->cache_enabled) {
		ImageRefMap::iterator entry = images.find(name);
		if (entry != images.end())
			return entry->second;
	}

	BufferPtr buffer(read(name));
	if (!buffer)
		return ImageRef();
	Gosu::Bitmap bitmap;
	Gosu::loadImageFile(bitmap, buffer->frontReader());
	ImageRef result(new Gosu::Image(window->graphics(), bitmap, false));
	images[name] = result;
	return result;
}

void Resourcer::getBitmap(Gosu::Bitmap& bitmap, const std::string& name)
{
	BufferPtr buffer(read(name));
	if (!buffer)
		return;
	return Gosu::loadImageFile(bitmap, buffer->frontReader());
}

Gosu::Image* Resourcer::bitmapSection(const Gosu::Bitmap& src,
        unsigned x, unsigned y, unsigned w, unsigned h, bool tileable)
{
	return new Gosu::Image(window->graphics(), src, x, y, w, h, tileable);
}

/* FIXME
 * We use Gosu::Sample for music because Gosu::Song's SDL implementation
 * doesn't support loading from a memory buffer at the moment.
 */
SampleRef Resourcer::getSample(const std::string& name)
{
	if (conf->cache_enabled) {
		SampleRefMap::iterator entry = samples.find(name);
		if (entry != samples.end())
			return entry->second;
	}

	BufferPtr buffer(read(name));
	if (!buffer)
		return SampleRef();
	SampleRef result(new Gosu::Sample(buffer->frontReader()));
	samples[name] = result;
	return result;
}

XMLDocRef Resourcer::getXMLDoc(const std::string& name, const std::string& dtdPath)
{
	if (conf->cache_enabled) {
		XMLMap::iterator entry = xmls.find(name);
		if (entry != xmls.end())
			return entry->second;
	}

	XMLDocRef result(readXMLDocFromDisk(name, dtdPath));
	xmls[name] = result;
	return result;
}

// use RAII to ensure doc is freed
// boost::shared_ptr<void> alwaysFreeTheDoc(doc, xmlFreeDoc);
xmlDoc* Resourcer::readXMLDocFromDisk(const std::string& name, const std::string& dtdPath)
{
	const std::string docStr = readStringFromDisk(name);
	if (docStr.empty())
		return NULL;

	xmlParserCtxt* pc = xmlNewParserCtxt();
	const std::string pathname = path(name);
	pc->vctxt.userData = (void*)&pathname;
	pc->vctxt.error = xmlErrorCb;

	// Parse the XML. Hand over our error callback fn.
	xmlDoc* doc = xmlCtxtReadMemory(pc, docStr.c_str(),
			(int)docStr.size(), NULL, NULL,
			XML_PARSE_NOBLANKS |
			XML_PARSE_NONET);
	xmlFreeParserCtxt(pc);
	if (!doc) {
		Log::err(pathname, "Could not parse file");
		return NULL;
	}

	// Load up a Document Type Definition for validating the document.
	xmlDtd* dtd = xmlParseDTD(NULL, (const xmlChar*)dtdPath.c_str());
	if (!dtd) {
		Log::err(dtdPath, "file not found");
		return NULL;
	}

	// Assert the document is sane here and now so we don't have to have a
	// billion if-else statements while traversing the document tree.
	xmlValidCtxt* vc = xmlNewValidCtxt();
	int valid = xmlValidateDtd(vc, doc, dtd);
	xmlFreeValidCtxt(vc);
	xmlFreeDtd(dtd);

	if (!valid) {
		Log::err(pathname, "XML document does not follow DTD");
		return NULL;
	}

	return doc;
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

std::string Resourcer::path(const std::string& entry_name) const
{
	return conf->world + "/" + entry_name;
}

