/****************************
** Tsunagari Tile Engine   **  
** os-windows.h            **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef OS_WINDOWS_H
#define OS_WINDOWS_H
#ifdef _WIN32

#include <string>
#include <Windows.h>

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

	/* Fix PI declaration for VisualC++. */
	#ifndef M_PI
		#define M_PI 3.14159265358979323846
		#define M_PI_2 1.57079632679489661923
	#endif
// ===

//! Allow console output on Windows when Tsunagari is run from the console.
void wFixConsole();

//! Create a halting (modal) message box.
void wMessageBox(std::string title, std::string text);

#endif
#endif

