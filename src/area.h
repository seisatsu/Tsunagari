/******************************
** Tsunagari Tile Engine     **
** area.h                    **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef AREA_H
#define AREA_H

#include <string>
#include <vector>

#include <Gosu/Gosu.hpp>

#include "common.h"

class Entity;
class GameWindow;
class TileMatrix;
class Resourcer;

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
	bool processDescriptor();
	
	Resourcer* rc;
	Entity* player;
	TileMatrix* tiles;
	const std::string descriptor;
	
	//! AreaEventTriggerType XML Storage Enum
	/*!
	  Stores the trigger type for an event.
	*/
	enum AreaEventTriggerType {
		LOAD,
		ENTER,
		LEAVE,
		DOOR
	};
	
	//! AreaEventTileFlag XML Storage Enum
	/*!
	  Stores the flag type for a tile flag.
	*/
	enum AreaMapTileFlag {
		NOWALK
	};
	
	//! AreaMusic XML Storage Struct
	/*!
	  Stores information for a <music> tag in Area.
	*/
	struct AreaMusic {
		bool loop;
		std::string file;
	};
	
	//! AreaEvent XML Storage Struct
	/*!
	  Stores information for an <event> tag in Area.
	*/
	struct AreaEvent {
		AreaEventTriggerType trigger;
		std::string name;
	};
	
	//! AreaMapTile XML Storage Struct
	/*!
	  Stores XML information for tiles in a map layer.
	*/
	struct AreaMapTile {
		std::string image;
		std::vector<AreaMapTileFlag> flags;
		std::vector<AreaEvent> events;
	};
	
	//! AreaMapGrid XML Storage Struct
	/*!
	  Stores XML information for grid layers in Area's tile map.
	*/
	struct AreaMapGrid {
		int layer;
		std::vector<std::vector<AreaMapTile> > rows;
	};
	
	//! AreaValues XML Storage Struct
	/*!
		Main XML storage struct for Area.
	*/
	struct AreaValues {
		std::string name;
		std::string author;
		std::string tileset;
		AreaMusic intro;
		AreaMusic main;
		std::vector<AreaEvent> onLoadEvents;
		std::vector<AreaMapGrid> map;
	} xml;
};

#endif

