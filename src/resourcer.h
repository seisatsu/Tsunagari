/*********************************
** Tsunagari Tile Engine        **
** resourcer.h                  **
** Copyright 2011-2012 OmegaSDG **
*********************************/

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

#ifndef RESOURCER_H
#define RESOURCER_H

#include <deque>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <libxml/parser.h>
#include <Python.h>

#include "cache.h"
#include "image.h"
#include "sound.h"
#include "tiledimage.h"
#include "xml.h"

namespace Gosu {
	class Buffer;
	class Sample;
	class Song;
}

// We hand out and manage resources in these forms:
typedef boost::shared_ptr<Sound> SampleRef;
typedef boost::shared_ptr<Gosu::Song> SongRef;
typedef boost::shared_ptr<XMLDoc> XMLRef;
typedef boost::shared_ptr<xmlDtd> DTDRef;
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
	bool prependPath(const std::string& path);
	bool appendPath(const std::string& path);
	bool rmPath(const std::string& path);

	//! Returns true if the World contains a resource by that name.
	bool resourceExists(const std::string& name) const;

	//! Request an image from the World.
	ImageRef getImage(const std::string& name);

	//! Request an image resource from the World and splits it into a
	//! number of tiles that each have width and height w by h.
	TiledImageRef getTiledImage(const std::string& name,
		int w, int h);

	//! Request a sound object from the World. The sound will be
	//! completely loaded into memory at once.
	SampleRef getSample(const std::string& name);

	//! Request a music stream from the World. The stream will be
	//! loaded from disk as it is being played.
	SongRef getSong(const std::string& name);

	//! Request an XML document from the World.
	XMLRef getXMLDoc(const std::string& name,
		const std::string& dtdPath);

	//! Request a Python script from the World be run.
	bool runPythonScript(const std::string& name);

	//! Request a text file from the World.
	std::string getText(const std::string& name);

	//! Expunge old resources cached in memory. Decisions on which are
	//! removed and which are kept are based on the global Conf struct.
	void garbageCollect();

private:
	DTDRef parseDTD(const std::string& path);
	bool preloadDTDs();

	//! Read an XML document from the game archive and parse it.
	XMLDoc* readXMLDoc(const std::string& name,
		const std::string& dtdFile);

	//! Requests an XML DTD from Tsunagari. (Does not load from the World.)
	xmlDtd* getDTD(const std::string& name);

	//! Read a generic resource from the game archive.
	Gosu::Buffer* readBuffer(const std::string& name);

	//! Read a string resource from the game archive.
	std::string readString(const std::string& name);

	//! Reads a file from the game archive.
	template <class T>
	bool readFromDisk(const std::string& name, T& buf);

	//! Build a pathname: arhive + "/" + entryName
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

	// DTDs don't expire. No garbage collection.
	typedef boost::unordered_map<std::string, DTDRef> TextMap;
	TextMap dtds;
};

void exportResourcer();

#endif

