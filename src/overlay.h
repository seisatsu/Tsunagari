/*********************************
** Tsunagari Tile Engine        **
** overlay.h                    **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef OVERLAY_H
#define OVERLAY_H

#include "entity.h"

class Overlay : public Entity
{
public:
	Overlay();
	virtual ~Overlay();

	void teleport(int x, int y);
	void move(int x, int y);

protected:
};

#endif

