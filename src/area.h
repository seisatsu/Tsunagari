/******************************
** Tsunagari Tile Engine     **
** area.h                    **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef AREA_H
#define AREA_H

#include <string>

#include <Gosu/Gosu.hpp>

#include "common.h"
#include "entity.h"
#include "resourcer.h"
//#include "tilegrid.h"
#include "tile.h"

class Entity;
class GameWindow;
class Resourcer;
class Tile;

//! Area Class
/*!
	This class is responsible for each map, or area in a Tsunagari World.
*/
class Area
{
public:
	//! Area Constructor
	Area(Resourcer* rc, Entity* player, const std::string filename);
	
	//! Area Destructor
	~Area();
	
	bool init();
	
	//! Gosu Callback
	void buttonDown(const Gosu::Button btn);
	
	//! Gosu Callback
	void draw();
	
	//! Gosu Callback
	bool needsRedraw() const;
	
	coord_t translateCoords(); //XXX What does this do?

private:
//	void build(Resourcer* rc, yaml_t* yaml);
	Resourcer* rc;
	Entity* player;
	Tile* t;
//	TileGrid* grid;
};

#endif

