/*********************************
** Tsunagari Tile Engine        **
** os-windows.h                 **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef OS_WINDOWS_H
#define OS_WINDOWS_H

#include <string>
#include <Windows.h>

//! Allow console output on Windows when Tsunagari is run from the console.
void wFixConsole();

//! Create a halting (modal) message box.
void wMessageBox(std::string title, std::string text);

#endif
