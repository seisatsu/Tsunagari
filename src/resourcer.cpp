/******************************
** Tsunagari Tile Engine     **
** resourcer.cpp             **
** Copyright 2011 OmegaSDG   **
******************************/

#include <errno.h>
#include <stdlib.h>

#include <Gosu/Audio.hpp>
#include <Gosu/Bitmap.hpp>
#include <Gosu/Image.hpp>
#include <Gosu/IO.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

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

Resourcer::Resourcer(GameWindow* window, const std::string& zip_filename)
	: window(window), z(NULL), zip_filename(zip_filename)
{
}

Resourcer::~Resourcer()
{
	if (z && zip_close(z))
		Log::err(zip_filename,
		         std::string("closing : ") + zip_strerror(z));
}

bool Resourcer::init()
{
	int err;
	z = zip_open(zip_filename.c_str(), 0x0, &err);
	if (!z) {
		char buf[512];
		zip_error_to_str(buf, sizeof(buf), err, errno);
		Log::err(zip_filename, buf);
		return false;
	}

	return true;
}

Gosu::Image* Resourcer::getImage(const std::string& name)
{
	boost::scoped_ptr<Gosu::Buffer> buffer(read(name));
	if (!buffer)
		return NULL;
	Gosu::Bitmap bitmap;
	Gosu::loadImageFile(bitmap, buffer->frontReader());
	return new Gosu::Image(window->graphics(), bitmap, false);
}

void Resourcer::getBitmap(Gosu::Bitmap& bitmap, const std::string& name)
{
	boost::scoped_ptr<Gosu::Buffer> buffer(read(name));
	if (!buffer)
		return;
	return Gosu::loadImageFile(bitmap, buffer->frontReader());
}

Gosu::Image* Resourcer::bitmapSection(const Gosu::Bitmap& src,
        unsigned x, unsigned y, unsigned w, unsigned h, bool tileable)
{
	return new Gosu::Image(window->graphics(), src, x, y, w, h, tileable);
}


xmlDoc* Resourcer::getXMLDoc(const std::string& name)
{
	const std::string docStr = getString(name);
	if (docStr.empty())
		return NULL;

	xmlParserCtxt* ctxt = xmlNewParserCtxt();
	const std::string pathname = path(name);
	ctxt->vctxt.userData = (void*)&pathname;
	ctxt->vctxt.error = xmlErrorCb;
	boost::shared_ptr<void> ctxtDeleter(ctxt, xmlFreeParserCtxt);

	xmlDoc* doc = xmlCtxtReadMemory(ctxt, docStr.c_str(),
			(int)docStr.size(), NULL, NULL,
			XML_PARSE_NOBLANKS |
			XML_PARSE_NONET |
			XML_PARSE_DTDVALID);
	if (!doc) {
		Log::err(pathname, "Could not parse file");
		return NULL;
	}
	else if (!ctxt->valid) {
		Log::err(pathname, "XML document does not follow DTD");
		return NULL;
	}

	return doc;
}

/* FIXME
 * We use Gosu::Sample for music because Gosu::Song's SDL implementation
 * doesn't support loading from a memory buffer at the moment.
 */
Gosu::Sample* Resourcer::getSample(const std::string& name)
{
	Gosu::Buffer* buffer = read(name);
	if (!buffer)
		return NULL;
	return new Gosu::Sample(buffer->frontReader());
}

std::string Resourcer::getString(const std::string& name)
{
	strMap::iterator entry = strings.find(name);
	if (entry != strings.end())
		return entry->second;

	std::string result = getStringFromZip(name);

	strings[name] = result;
	return result;
}

std::string Resourcer::getStringFromZip(const std::string& name)
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

xmlDoc* Resourcer::getXMLDoc(const std::string& name)
{
	const std::string docStr = getString(name);
	if (docStr.empty())
		return NULL;

	xmlParserCtxt* ctxt = xmlNewParserCtxt();
	const std::string pathname = path(name);
	ctxt->vctxt.userData = (void*)&pathname;
	ctxt->vctxt.error = xmlErrorCb;
	xmlDoc* doc = xmlCtxtReadMemory(ctxt, docStr.c_str(), (int)docStr.size(),
			NULL, NULL,
			XML_PARSE_NOBLANKS | XML_PARSE_NONET | XML_PARSE_DTDVALID);
	if (!doc) {
		xmlFreeParserCtxt(ctxt);
		Log::err(pathname, "Could not parse file");
		return NULL;
	}
	else if (!ctxt->valid) {
		xmlFreeParserCtxt(ctxt);
		Log::err(pathname, "XML document does not follow DTD");
		return NULL;
	}

	xmlFreeParserCtxt(ctxt);
	return doc;
}

/* FIXME
 * We use Gosu::Sample for music because Gosu::Song's SDL implementation
 * doesn't support loading from a memory buffer at the moment.
 */
Gosu::Sample* Resourcer::getSample(const std::string& name)
{
	boost::scoped_ptr<Gosu::Buffer> buffer(read(name));
	if (!buffer.get())
		return NULL;
	return new Gosu::Sample(buffer->frontReader());
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
		delete buffer;
		Log::err(path(name), "reading didn't complete");
		zip_fclose(zf);
		return NULL;
	}

	zip_fclose(zf);
	return buffer;
}

std::string Resourcer::path(const std::string& entry_name) const
{
	return zip_filename + "/" + entry_name;
}

void Resourcer::cacheTally(const std::string& key, const int mod)
{
	cache[key].first += mod;
	if (cache[key].first < 0)
		Log::dev(key, "cache underrun of "+itostr(cache[key].first));
}

