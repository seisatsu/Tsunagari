/****************************
** Tsunagari Tile Engine   **
** os-windows.h            **
** Copyright 2012 OmegaSDG **
****************************/

#if defined _WIN32 && !defined OS_WINDOWS_H
#define OS_WINDOWS_H

#include <string>

// === Windows Fixes ===
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
// ===

//! Allow console output on Windows when Tsunagari is run from the console.
void wFixConsole();

//! Create a halting (modal) message box.
void wMessageBox(const std::string& title, const std::string& text);

#endif

