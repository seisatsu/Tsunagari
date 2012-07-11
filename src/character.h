/*********************************
** Tsunagari Tile Engine        **
** character.h                  **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef CHARACTER_H
#define CHARACTER_H

#include "entity.h"

class Character : public Entity
{
public:
	Character();
	virtual ~Character();

	void teleport(int x, int y);
	void move(int x, int y);

protected:
	virtual void erase();
};

#endif

