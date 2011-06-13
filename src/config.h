/******************************
** Tsunagari Tile Engine     **
** config.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef CONFIG_H
#define CONFIG_H

// === Default Configuration Settings ===
	/* Tsunagari config file. */
	#define CLIENT_CONF_FILE "./client.conf"
	
	/* Error verbosity level. */
	#define MESSAGE_MODE MM_DEBUG
	
	/* Milliseconds of button down before starting persistent input in
	   roguelike movement mode. */
	#define ROGUELIKE_PERSIST_DELAY_INIT 500
	
	/* Milliseconds between persistent input sends in roguelike movement
	   mode. */
	#define ROGUELIKE_PERSIST_DELAY_CONSECUTIVE 100
	
	/* Time to live in seconds for empty resource cache entries. */
	#define CACHE_EMPTY_TTL "300"
// ===

// === Platform Conditionals ===
	/* Placate silly complaints from Visual C++. */
	#ifdef _MSC_VER
		#define snprintf _snprintf
	#endif
// ===

#endif

