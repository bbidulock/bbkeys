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
	setHidable(True);
	setAlignment(AlignBottom);
	defaultMenu();
}

Stackmenu::~Stackmenu() {
}

void Stackmenu::reconfigure()
{
	setMenuItems();
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
			XGetWMName(bbtool->getXDisplay(), it.current()->win, &xtp);
			XTextPropertyToStringList(&xtp, &windowname, &num);
			insert((char*) *windowname);
			XFreeStringList(windowname);
		}
	}
}

void Stackmenu::clearMenu() {
	while (getCount())
		remove(0);
}

void Stackmenu::Update() {
	wait_for_update=False;
	Basemenu::update();	
}

void Stackmenu::itemSelected(int button, int index)
{
}

void Stackmenu::key_release(unsigned int key)
{
	if (key == 64) {
printf("Alt Released\n");
		int selected = menuPosition;
		LinkedListIterator<WindowList> it(bbtool->windowList);
		for(; it.current(); it++)
			if (it.current()->desktop == bbtool->getCurrentDesktopNr())
				if(!selected--) {
					bbtool->wminterface->setWindowFocus(it.current()->win);
					XRaiseWindow(bbtool->getXDisplay(), it.current()->win);
				}
		XUngrabKeyboard(bbtool->getXDisplay(), CurrentTime);
		hide();
	}
}

void Stackmenu::key_press(int grabInt)
{
	switch (grabInt) {
		case grabNextWindow:
			if(++menuPosition >= getCount())
				menuPosition = 0;
			setHighlight(menuPosition);
			break;
		case grabPrevWindow:
			if(--menuPosition < 0)
				menuPosition = getCount() - 1;
			setHighlight(menuPosition);
			break;
		default:
			XUngrabKeyboard(bbtool->getXDisplay(), CurrentTime);
			hide();
			break;
	}
}

void Stackmenu::show(bool forward)
{
printf("in Stackmenu::show()\n");
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
/*	if (forward) {
  	if(++menuPosition >= getCount())
    	menuPosition = 0;
	} else {
		if(--menuPosition < 0)
			menuPosition = getCount() - 1;
  }
	setHighlight(menuPosition);*/
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
