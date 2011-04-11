/******************************
** Tsunagari Tile Engine     **
** resourcer.cpp             **
** Copyright 2011 OmegaSDG   **
******************************/

#include <stdlib.h>
#include <stdio.h>

#include <Gosu/Utility.hpp>
#include <libmpq/mpq.h>

#include "log.h"
#include "resourcer.h"

Resourcer::Resourcer(GameWindow* window, const std::string& filename)
	: window(window), filename(filename + ".mpq")
{
}

Resourcer::~Resourcer()
{
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
	mpq_archive *a;
	uint32_t file_index;
	off_t size;

	if (libmpq__archive_open(&a, filename.c_str(), -1)) {
		Log::err("read", "opening");
		exit(1);
	}

	if (libmpq__file_number(a, name.c_str(), &file_index)) {
		Log::err("read", "file index");
		exit(1);
	}
	libmpq__file_unpacked_size(a, file_index, &size);

	buffer->resize(size);
	libmpq__file_read(a, file_index, (uint8_t*)buffer->data(), size, NULL);

	libmpq__archive_close(a);
}

std::string Resourcer::getString(const std::string& name)
{
	mpq_archive *a;
	uint32_t file_index;
	off_t size;
	char *buf;
	std::string str;

	if (libmpq__archive_open(&a, filename.c_str(), -1)) {
		Log::err("read", "opening");
		exit(1);
	}

	if (libmpq__file_number(a, name.c_str(), &file_index)) {
		Log::err("read", "file index");
		exit(1);
	}
	libmpq__file_unpacked_size(a, file_index, &size);

	buf = new char[size + 1];
	buf[size] = '\0';
	libmpq__file_read(a, file_index, (uint8_t*)buf, size, NULL);
	str = buf;
	delete[] buf;

	libmpq__archive_close(a);

	return str;
}

