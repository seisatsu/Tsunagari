/*********************************
** Tsunagari Tile Engine        **
** area-tmx.h                   **
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

#ifndef AREA_TMX_H
#define AREA_TMX_H

#include <string>

#include "area.h"
#include "tile.h"
#include "xml.h"

class Viewport;
class Player;

class AreaTMX : public Area
{
public:
	AreaTMX(Viewport* view, Player* player, const std::string& filename);
	virtual ~AreaTMX();

	//! Parse the file specified in the constructor, generating a full Area
	//! object. Must be called before use.
	virtual bool init();

private:
	//! Allocate Tile objects for one layer of map.
	void allocateMapLayer();

	//! Parse an Area file.
	bool processDescriptor();
	bool processMapProperties(XMLNode node);
	bool processTileSet(XMLNode node);
	bool processTileType(XMLNode node, TileType& type,
			TiledImageRef& img, int id);
	bool processLayer(XMLNode node);
	bool processLayerProperties(XMLNode node, double* depth);
	bool processLayerData(XMLNode node, int z);
	bool processObjectGroup(XMLNode node);
	bool processObjectGroupProperties(XMLNode node, double* depth);
	bool processObject(XMLNode node, int z);
	bool splitTileFlags(const std::string& strOfFlags, unsigned* flags);
	bool parseExit(const std::string& dest, Exit* exit,
		bool* wwide, bool* hwide);
	bool parseRGBA(const std::string& str,
		Gosu::Color::Channel* r,
		Gosu::Color::Channel* g,
		Gosu::Color::Channel* b,
		Gosu::Color::Channel* a);

	std::vector<TileType*> gids;
};

#endif

