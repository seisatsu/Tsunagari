/****************************
** Tsunagari Tile Engine   **
** os-windows.cpp          **
** Copyright 2012 OmegaSDG **
****************************/

#ifdef _WIN32

#include "os-windows.h"
#include "window.h"
#include "world.h"

#include <Gosu/Gosu.hpp>
#include <Windows.h>

void wFixConsole()
{
	if (AttachConsole(ATTACH_PARENT_PROCESS)) {
		freopen("CONOUT$","wb",stdout);
		freopen("CONOUT$","wb",stderr);
	}
}

void wMessageBox(const std::string& title, const std::string& text)
{
	World::instance()->setPaused(true);
	MessageBox(GameWindow::instance().handle(),
		Gosu::widen(text).c_str(),
		Gosu::widen(title).c_str(),
		MB_OK
	);
	World::instance()->setPaused(false);
}

#endif

