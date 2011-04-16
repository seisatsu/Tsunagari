/******************************
** Tsunagari Tile Engine     **
** resourcer.cpp             **
** Copyright 2011 OmegaSDG   **
******************************/

#include <stdlib.h>
#include <stdio.h>

#include <Gosu/Utility.hpp>
#include <zip.h>

#include "log.h"
#include "resourcer.h"

Resourcer::Resourcer(GameWindow* window, const std::string& filename)
	: window(window), filename(filename)
{
}

Resourcer::~Resourcer()
{
}

std::string Resourcer::tpkgName()
{
	return filename;
}

Gosu::Image* Resourcer::getImage(const std::string& name)
{
	Gosu::Buffer buffer;
	read(name, &buffer);

	Gosu::Bitmap bitmap = Gosu::loadImageFile(buffer.frontReader());
//	Gosu::Image image(window->graphics(), bitmap, false);

	return new Gosu::Image(window->graphics(), bitmap, false);
//	return new Gosu::Image(window->graphics(), Gosu::utf8ToWstring(name),
//			false);
}

// XXX: Can we just return Gosu::Buffer type?
void Resourcer::read(const std::string& name, Gosu::Buffer* buffer)
{
	zip* z;
	struct zip_stat stat;
	zip_file* zf;
	int size;

	z = zip_open(filename.c_str(), 0x0, NULL);
	if (!z) {
		Log::dbg("Resourcer::read", "zip_open failed.");
		return;
	}

	if (zip_stat(z, name.c_str(), 0x0, &stat)) {
		zip_close(z);
		Log::dbg("Resourcer::read", "zip_stat failed.");
		return;
	}

	size = stat.size;
	buffer->resize(size);

	zf = zip_fopen(z, name.c_str(), 0x0);
	if (!zf) {
		zip_close(z);
		Log::dbg("Resourcer::read", "zip_fopen failed.");
		return;
	}

	if (zip_fread(zf, buffer->data(), size) != size) {
		zip_fclose(zf);
		zip_close(z);
		Log::dbg("Resourcer::read", "zip_fread failed.");
		return;
	}

	zip_fclose(zf);
	zip_close(z);
}

std::string Resourcer::getString(const std::string& name)
{
	zip* z;
	struct zip_stat stat;
	zip_file* zf;
	int size;
	char* buf;
	std::string str;

	z = zip_open(filename.c_str(), 0x0, NULL);
	if (!z) {
		Log::dbg("Resourcer::getString", "zip_open failed.");
		return "";
	}

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
	zip_close(z);

	return str;
}

