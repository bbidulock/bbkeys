// cycling.cc for bbkeys
//
//	Copyright (c) 2001 by Ben Jansens <xor@x-o-r.net>
//
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// (See the included file COPYING / GPL-2.0)
//
#include "bbkeys.hh"
#include "LinkedList.hh"

#include <X11/Xutil.h>

Stackmenu::Stackmenu(ToolWindow *tool) :
		Basemenu(tool)
{
	bbtool = tool;

	setTitleVisibility(False);
	setMovable(False);
	setAlignment(AlignBottom);
}

Stackmenu::~Stackmenu() {
}

void Stackmenu::itemSelected(int,int) {
}

void Stackmenu::reconfigure()
{
	Basemenu::reconfigure();
}

void Stackmenu::setMenuItems() {
	XTextProperty xtp;
	char **windowname;
	int num;
	LinkedListIterator<WindowList> it(bbtool->windowList);

	clearMenu();

	for (; it.current(); it++) {
		if (it.current()->desktop == bbtool->getCurrentDesktopNr()) {
			if (!XGetWMName(bbtool->getXDisplay(), it.current()->win, &xtp))
				if (!XTextPropertyToStringList(&xtp, &windowname, &num)) {
					if (*windowname) insert((char*) *windowname);
					if (windowname) XFreeStringList(windowname);
				}
		}
	}
	centerPosition();
}

void Stackmenu::clearMenu() {
	while (getCount())
		remove(0);
}

void Stackmenu::Update() {
	wait_for_update=False;
	Basemenu::update();	
}

void Stackmenu::key_release(unsigned int key)
{
	if (key == 64) {
		int selected = menuPosition;
		LinkedListIterator<WindowList> it(bbtool->windowList);
		for(; it.current(); it++)
			if (it.current()->desktop == bbtool->getCurrentDesktopNr())
				if(!selected--) {
					bbtool->wminterface->setWindowFocus(it.current()->win);
					XRaiseWindow(bbtool->getXDisplay(), it.current()->win);
				}
		hide();
	}
}

void Stackmenu::key_press(int grabInt)
{
	switch (grabInt) {
		case grabNextWindow:
			if(++menuPosition >= getCount())
				menuPosition = 0;
      setSelected(menuPosition);
			break;
		case grabPrevWindow:
			if(--menuPosition < 0)
				menuPosition = getCount() - 1;
      setSelected(menuPosition);
			break;
		default:
			hide();
			break;
	}
}

void Stackmenu::hide()
{
	XUngrabKeyboard(bbtool->getXDisplay(), CurrentTime);
	Basemenu::hide();
}	

void Stackmenu::show(bool forward)
{
	setMenuItems();
	if (getCount() < 1)	return;	// no windows, just return
	if (getCount() == 1) {
		// only 1 window, focus it and leave
		LinkedListIterator<WindowList> it(bbtool->windowList);
		for (; it.current(); it++)
			if (it.current()->desktop == bbtool->getCurrentDesktopNr())
				bbtool->wminterface->setWindowFocus(it.current()->win);
		return;
	}

	XRaiseWindow(bbtool->getXDisplay(), getWindowID());
	/*
	XGrabKey(bbtool->getXDisplay(), 64, 0, 
			bbtool->getScreenInfo(0)->getRootWindow(), True,
			GrabModeSync, GrabModeAsync);
	*/
	XGrabKeyboard(bbtool->getXDisplay(), 
			bbtool->getScreenInfo(0)->getRootWindow(), True,
			GrabModeAsync, GrabModeAsync, CurrentTime);

	menuPosition = 0;
	key_press(forward?grabNextWindow:grabPrevWindow);

	Basemenu::show();
}

void Stackmenu::centerPosition()
{
	int x = (bbtool->getCurrentScreenInfo()->getWidth()>>1) - (getWidth()>>1);
	int y = (bbtool->getCurrentScreenInfo()->getHeight()>>1) - (getHeight()>>1);
	if (x>0 && y>0)
		Basemenu::move(x, y);
}

void Stackmenu::setSelected(int sel)
{
	if ((sel<0) || (sel>=getCount())) return;
	setHighlight(sel);
	update();
}
