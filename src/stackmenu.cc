// cycling.cc for bbkeys
//
//	Copyright (c) 2001 by Ben Jansens <xor@orodu.net>
//	Copyright (c) 2001 by Jason Kasper (vanRijn) <vR@movingparts.net>
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
// $Id: stackmenu.cc,v 1.7 2002/05/30 18:26:25 eckzor Exp $

#include "bbkeys.hh"
#include "LinkedList.hh"

#include <X11/Xutil.h>

#ifdef    HAVE_CONFIG_H
#  include "../config.h"
#endif // HAVE_CONFIG_H

Stackmenu::Stackmenu(ToolWindow *tool) :
		Basemenu(tool)
{
	bbtool = tool;

	setTitleVisibility(False);
	setMovable(False);
	setAlignment(AlignBottom);
	XSelectInput(bbtool->getXDisplay(), getWindowID(), VisibilityChangeMask);
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
	XTextProperty text_prop;
	char **list;
	char *title=0;
	int num;

	LinkedListIterator<WindowList> it(bbtool->windowList);

	clearMenu();
	for (; it.current(); it++) {
		if (((bbtool->getShowAllWorkspaces()) &&
				((!it.current()->sticky) ||
				(it.current()->desktop == bbtool->getCurrentDesktopNr())))
				||
				((!bbtool->getShowAllWorkspaces()) &&
				(it.current()->desktop == bbtool->getCurrentDesktopNr()))) {

			if (title) {
				delete [] title;
				title = 0;
			}

			// what's our window name?
			if (XGetWMName(bbtool->getXDisplay(), it.current()->win, &text_prop)) {
				if (text_prop.value && text_prop.nitems > 0) {
					if (text_prop.encoding != XA_STRING) {
						text_prop.nitems = strlen((char *) text_prop.value);
			
						if ((XmbTextPropertyToTextList(bbtool->getXDisplay(), &text_prop,
								&list, &num) == Success) &&
								(num > 0) && *list) {
							title = bstrdup(*list);
							XFreeStringList(list);
						} else
							title = bstrdup((char *) text_prop.value);
					} else
						title = bstrdup((char *) text_prop.value);
			
					XFree((char *) text_prop.value);
				} else
					title = bstrdup( "Unnamed");
			} else
				title = bstrdup( "Unnamed");

			// now tack on the workspace name if we need to
			if (bbtool->getShowAllWorkspaces()) {
				int size = strlen(title) + strlen(" (workspace )") + 2;
				char *workspace = (char*) malloc(size);
				if (workspace) {
					snprintf(workspace, size, "%s (workspace %i)", title,
							it.current()->desktop+1);
					insert(workspace);
					free(workspace);
				}
			} else
				insert(title);
		}
	}
}

void Stackmenu::clearMenu() {
	while (getCount())
		remove(0);
}

void Stackmenu::selectFocused(bool raise)
{
	int selected = menuPosition;
	LinkedListIterator<WindowList> it(bbtool->windowList);
	for(; it.current(); it++) 
		if (((bbtool->getShowAllWorkspaces()) &&
				((!it.current()->sticky) ||
				(it.current()->desktop == bbtool->getCurrentDesktopNr())))
				||
				((!bbtool->getShowAllWorkspaces()) &&
				(it.current()->desktop == bbtool->getCurrentDesktopNr()))) {
			if(!selected--) {
				bbtool->wminterface->setWindowFocus(it.current()->win);
				if ( raise ) {
					// okay, an explanation is in order... First, we have to
					// hide our window so that focusWindow() actually does
					// anything.
					// Next, if the window to focus is on a different desktop than
					// the current one, we need to switch there.
					// Then we XRaiseWindow, because if we did
					// focusWindow() first, we'd then XRaise() the wrong window.
					// Lastly, we update bbkey's stack with what we just
					// raised...
					hide();
					if (bbtool->getCurrentDesktopNr() != it.current()->desktop)
						bbtool->wminterface->changeDesktop(it.current()->desktop, False);
					XRaiseWindow(bbtool->getXDisplay(), it.current()->win);
					bbtool->focusWindow(it.current()->win);
				}
			}
		}
		if ( raise ) {
			hide();
	}
}

void Stackmenu::key_press(int grabInt)
{
	switch (grabInt) {
		case grabNextWindow:
		case grabNextWindowAllWorkspaces:
			if(++menuPosition >= getCount())
				menuPosition = 0;
			setSelected(menuPosition);
			selectFocused(False);
			break;
		case grabPrevWindow:
			if(--menuPosition < 0)
				menuPosition = getCount() - 1;
			setSelected(menuPosition);
			selectFocused(False);
			break;
		default:
			break;
	}
}

void Stackmenu::hide()
{
	XUngrabKeyboard(bbtool->getXDisplay(), CurrentTime);
	Basemenu::hide();
	bbtool->setCycling(False);
}	


void Stackmenu::show(void)
{
	Basemenu::show();
}


void Stackmenu::show(bool forward, bool showMenu)
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
	XGrabKeyboard(bbtool->getXDisplay(), 
			bbtool->getScreenInfo(0)->getRootWindow(), True,
			GrabModeAsync, GrabModeAsync, CurrentTime);

	menuPosition = 0;
	key_press(forward?grabNextWindow:grabPrevWindow);

	bbtool->setCycling(True);

	if (showMenu) {
		show();
		centerPosition();
	}
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
