/*********************************
** Tsunagari Tile Engine        **
** resourcer.h                  **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef RESOURCER_H
#define RESOURCER_H

#include <deque>
#include <string>

#include <boost/shared_ptr.hpp>
#include <libxml/parser.h>
#include <Python.h>

#include "cache.h"
#include "common.h"
#include "sound.h"
#include "xml.h"

namespace Gosu {
	class Bitmap;
	class Buffer;
	class Image;
	class Sample;
	class Song;
}

using boost::shared_ptr;

// We hand out and manage resources in these forms:
typedef boost::shared_ptr<Gosu::Image> ImageRef;
typedef boost::shared_ptr<Sound> SampleRef;
typedef boost::shared_ptr<Gosu::Song> SongRef;
typedef boost::shared_ptr<XMLDoc> XMLRef;
typedef std::deque<ImageRef> TiledImage;
typedef boost::shared_ptr<TiledImage> TiledImageRef;
typedef boost::shared_ptr<std::string> StringRef;

/**
 * Provides data and resource extraction for a World.
 * Each World comes bundled with associated data in the form of a Zip file.
 * A Resourcer object knows how to navigate the data, extract individual
 * requested files, and process the files into data structures. The final data
 * structures are kept in memory for future requests.
 */
class Resourcer
{
public:
	//! Get the Resourcer for the current World.
	static Resourcer* instance();

	Resourcer();
	~Resourcer();
	bool init(char* argv0);

	//! Returns true if the World contains a resource by that name.
	bool resourceExists(const std::string& name) const;

	//! Request an image from the World.
	ImageRef getImage(const std::string& name);

	//! Request an image resource from the World and splits it into a
	//! number of tiles that each have width and height w by h. Returns
	//! false if the source image is not found.
	bool getTiledImage(TiledImage& img, const std::string& name,
		int w, int h, bool tileable);

	//! Request a sound object from the World. The sound will be
	//! completely loaded into memory at once.
	SampleRef getSample(const std::string& name);

	//! Request a music stream from the World. The stream will be
	//! loaded from disk as it is being played.
	SongRef getSong(const std::string& name);

	//! Request an XML document from the World.
	XMLRef getXMLDoc(const std::string& name,
		const std::string& dtdFile);

	//! Request a Python script from the World be run.
	bool runPythonScript(const std::string& name);

	//! Request a text file from the World.
	std::string getText(const std::string& name);

	//! Expunge old resources cached in memory. Decisions on which are
	//! removed and which are kept are based on the global Conf struct.
	void garbageCollect();

private:
	//! Read an XML document from disk and parse it.
	XMLDoc* readXMLDocFromDisk(const std::string& name,
		const std::string& dtdFile) const;

	//! Read a string resource from disk.
	std::string readStringFromDisk(const std::string& name) const;

	//! Read a generic resource from disk.
	Gosu::Buffer* read(const std::string& name) const;

	//! Helper function.
	std::string path(const std::string& entryName) const;

private:
	// Caches that store processed, game-ready objects. Garbage collected.
	Cache<ImageRef> images;
	Cache<TiledImageRef> tiles;
	Cache<SampleRef> sounds;
	Cache<SongRef> songs;
	Cache<XMLRef> xmls;
	Cache<PyCodeObject*> codes;
	Cache<StringRef> texts;
};

void exportResourcer();

#endif

