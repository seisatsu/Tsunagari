/*********************************
** Tsunagari Tile Engine        **
** area-tmx.h                   **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef AREA_TMX_H
#define AREA_TMX_H

#include <string>

#include "area.h"
#include "tile.h"
#include "xml.h"

class Viewport;
class Player;
class Music;

class AreaTMX : public Area
{
public:
	AreaTMX(Viewport* view, Player* player, Music* music,
	     const std::string& filename);
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
	bool processTileType(XMLNode node, TiledImage& img, int id);
	bool processLayer(XMLNode node);
	bool processLayerProperties(XMLNode node, double* depth);
	bool processLayerData(XMLNode node, int z);
	bool processObjectGroup(XMLNode node);
	bool processObjectGroupProperties(XMLNode node, double* depth);
	bool processObject(XMLNode node, int z);
	bool splitTileFlags(const std::string& strOfFlags, unsigned* flags);
	bool parseDoor(const std::string& dest, Door* door);
};

#endif

