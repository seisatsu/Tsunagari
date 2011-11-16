/******************************
** Tsunagari Tile Engine     **
** config.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef CONFIG_H
#define CONFIG_H

#define TSUNAGARI_RELEASE_VERSION "Tsunagari Tile Engine AlphaP3 Revision 1"

// === Default Configuration Settings ===
	/* Tsunagari config file. */
	#define CLIENT_CONF_FILE "./client.ini"

	/* Tsunagari XML DTD directory. */
	#define DTD_DIRECTORY "./dtd/"

	/* Error verbosity level. */
	#define MESSAGE_MODE MM_DEBUG

	/* Milliseconds of button down before starting persistent input in
	   roguelike movement mode. -- Move to World Descriptor */
	#define ROGUELIKE_PERSIST_DELAY_INIT 500

	/* Milliseconds between persistent input sends in roguelike movement
	   mode. -- Move to World Descriptor */
	#define ROGUELIKE_PERSIST_DELAY_CONSECUTIVE 100

	/* Time to live in seconds for empty resource cache entries before they
	   are deleted. */
	#define CACHE_EMPTY_TTL 300

	/* Maximum size in megabytes for the resource cache. */
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

	/* Fix PI declaration. */
	#ifndef M_PI
		#define M_PI 3.14159265358979323846
		#define M_PI_2 1.57079632679489661923
	#endif
// ===

#endif

