/******************************
** Tsunagari Tile Engine     **
** resourcer.cpp             **
** Copyright 2011 OmegaSDG   **
******************************/

#include <stdlib.h>

#include "log.h"
#include "resourcer.h"

Resourcer::Resourcer(GameWindow* window, const std::string& filename)
	: window(window), filename(filename), z(NULL)
{
}

Resourcer::~Resourcer()
{
	if (z) {
		zip_close(z);
	}
}

bool Resourcer::init()
{
	z = zip_open(filename.c_str(), 0x0, NULL);
	if (!z) {
		Log::err("Resourcer::init",
		         "Failed to open ZIP file: " + filename);
		return false;
	}

	return true;
}

const std::string Resourcer::getFilename()
{
	return filename;
}

Gosu::Image* Resourcer::getImage(const std::string& name)
{
	Gosu::Buffer buffer;
	if (!read(name, &buffer)) {
		return NULL;
	}
	Gosu::Bitmap bitmap = Gosu::loadImageFile(buffer.frontReader());
	return new Gosu::Image(window->graphics(), bitmap, false);
}

std::string Resourcer::getString(const std::string& name)
{
	struct zip_stat stat;
	zip_file* zf;
	int size;
	char* buf;
	std::string str;

	if (zip_stat(z, name.c_str(), 0x0, &stat)) {
		zip_close(z);
		Log::dbg("Resourcer::getString", "zip_stat failed.");
		return "";
	}

	size = stat.size;
	buf = new char[size + 1];
	buf[size] = '\0';

	zf = zip_fopen(z, name.c_str(), 0x0);
	if (!zf) {
		zip_close(z);
		Log::dbg("Resourcer::getString", "zip_fopen failed.");
		return "";
	}

	if (zip_fread(zf, buf, size) != size) {
		zip_fclose(zf);
		zip_close(z);
		Log::dbg("Resourcer::getString", "zip_fread failed.");
		return "";
	}

	str = buf;
	delete[] buf;

	zip_fclose(zf);

	return str;
}

// XXX: Can we just return Gosu::Buffer type?
bool Resourcer::read(const std::string& name, Gosu::Buffer* buffer)
{
	struct zip_stat stat;
	zip_file* zf;
	int size;

	if (zip_stat(z, name.c_str(), 0x0, &stat)) {
		Log::err("Resourcer::read", "ZIP entry not found: " + name);
		return false;
	}

	size = stat.size;
	buffer->resize(size);

	zf = zip_fopen(z, name.c_str(), 0x0);
	if (!zf) {
		Log::err("Resourcer::read",
		         "opening ZIP entry " + name + " failed: "
			   + zip_strerror(z));
		return false;
	}

	if (zip_fread(zf, buffer->data(), size) != size) {
		Log::err("Resourcer::read",
		         "reading ZIP entry " + name
			   + " failed: didn't complete");
		zip_fclose(zf);
		return false;
	}

	zip_fclose(zf);
	return true;
}

