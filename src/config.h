/******************************
** Tsunagari Tile Engine     **
** config.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef CONFIG_H
#define CONFIG_H

#define TSUNAGARI_RELEASE_VERSION "Tsunagari Tile Engine AlphaP1 Revision 6"

#define JUMP_MOVE  0 /* Roguelike */
#define SLIDE_MOVE 1 /* Yume Nikki-like */
#define FREE_MOVE  2 /* Zelda-like... not implemented yet */

// === Default Configuration Settings ===
	/* Tsunagari config file. -- Command Line */
	#define CLIENT_CONF_FILE "./client.ini"
	
	/* Error verbosity level. -- Command Line */
	#define MESSAGE_MODE MM_DEBUG
	
	/* Game movement mode */
	#define GAME_MODE SLIDE_MOVE

	/* Milliseconds of button down before starting persistent input in
	   roguelike movement mode. -- Move to World Descriptor */
	#define ROGUELIKE_PERSIST_DELAY_INIT 500
	
	/* Milliseconds between persistent input sends in roguelike movement
	   mode. -- Move to World Descriptor */
	#define ROGUELIKE_PERSIST_DELAY_CONSECUTIVE 100
	
	/* Time to live in seconds for empty resource cache entries before they
	   are deleted. -- Command Line */
	#define CACHE_EMPTY_TTL 300
	
	/* Maximum size in megabytes for the resource cache. -- Command Line */
	#define CACHE_MAX_SIZE 100
// ===

// === Platform Specifics ===
	/* Windows check. */
	#if defined(_Windows) && !defined(_WINDOWS)
		#define _WINDOWS 1
	#endif

	#if defined(_WINDOWS) && !defined(_Windows)
		#define _Windows 1
	#endif
	
	/* Fix snprintf for VisualC++. */
	#ifdef _MSC_VER
		#define snprintf _snprintf
	#endif
// ===

#endif

