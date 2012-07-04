/*********************************
** Tsunagari Tile Engine        **
** config.h                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef CONFIG_H
#define CONFIG_H

/* Release version. */
#define TSUNAGARI_RELEASE_VERSION "Tsunagari Tile Engine AlphaP4 Revision 2"

// === Required Data Paths ===
	/* Tsunagari config file. */
	#define CLIENT_CONF_PATH "./client.ini"

	/* Base data file. */
	#define BASE_ZIP_PATH "../data/base.zip"

	/* Tsunagari XML DTD directory. */
	#define XML_DTD_PATH "../data/dtd/"
// ===

// === Client.ini Default Values ===
	#define DEF_ENGINE_WORLD		"../data/testing.world"
	#define DEF_ENGINE_VERBOSITY	"verbose"
	#define DEF_WINDOW_WIDTH		640
	#define DEF_WINDOW_HEIGHT		480
	#define DEF_WINDOW_FULLSCREEN	false
	#define DEF_AUDIO_ENABLED		true
	#define DEF_CACHE_ENABLED		true
	#define DEF_CACHE_TTL			300
	#define DEF_CACHE_SIZE			100
// ===

// === Cross-Platform Fixes ===
	/* Fix snprintf for VisualC++. */
	#ifdef _MSC_VER
		#define snprintf _snprintf
	#endif

	/* Fix NAN constant for VisualC++. */
	#ifdef _MSC_VER
		#ifndef NAN
		    static const unsigned long __nan[2] = {0xffffffff, 0x7fffffff};
		    #define NAN (*(const float *) __nan)
		#endif
	#endif

	/* Fix PI declaration for VisualC++. */
	#ifndef M_PI
		#define M_PI 3.14159265358979323846
		#define M_PI_2 1.57079632679489661923
	#endif
// ===

#endif

