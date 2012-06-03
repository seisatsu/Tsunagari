/*********************************
** Tsunagari Tile Engine        **
** npc.h                        **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef NPC_H
#define NPC_H

#include "entity.h"

class NPC : public Entity
{
public:
	NPC();

	void postMove();

private:
	void takeExit(Exit* exit);
};

#endif

