//	wminterface.cc for bbtools.
//
//	Copyright (c) 1998-1999 by John Kennis, jkennis@chello.nl
//	Copyright (c) 2001 by Ben Jansens <xor@orodu.net>
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

#include "wminterface.hh"
#include "resource.hh"
#include "BaseDisplay.hh"

#ifdef    HAVE_CONFIG_H
#  include "../config.h"
#endif // HAVE_CONFIG_H

WMInterface::WMInterface(ToolWindow *toolwindow) : NETInterface(toolwindow) {
	bbtool=toolwindow;
	focusWindow = True;
}

WMInterface::~WMInterface() {
}

void WMInterface::sendClientMessage(Atom atom, XID data) {
	XEvent e;
	unsigned long mask;

	e.xclient.type = ClientMessage;
	e.xclient.window =	bbtool->getCurrentScreenInfo()->getRootWindow();
	e.xclient.message_type = atom;
	e.xclient.format = 32;
	e.xclient.data.l[0] = (unsigned long) data;
	e.xclient.data.l[1] = 0;

	mask =	SubstructureRedirectMask;
	XSendEvent(bbtool->getXDisplay(), 
						 bbtool->getCurrentScreenInfo()->getRootWindow(),
						 False, mask, &e);
}

void WMInterface::changeDesktop(int desk_number, bool focusWin) {
	sendClientMessage(bbtool->getBlackboxChangeWorkspaceAtom(),
		static_cast<unsigned long int>(desk_number));
	focusWindow = focusWin;
}

void WMInterface::sendWindowToDesktop(Window win, int desk_number) {
	XEvent e;
	unsigned long mask;

	e.xclient.type = ClientMessage;
	e.xclient.window = win;
	e.xclient.message_type = bbtool->getBlackboxChangeAttributesAtom();
	e.xclient.format = 32;
	e.xclient.data.l[0] = AttribWorkspace;
	e.xclient.data.l[2] = desk_number;
	e.xclient.data.l[1] = e.xclient.data.l[3] = 0; 
	e.xclient.data.l[4] = e.xclient.data.l[5] = 0;

	mask =	SubstructureRedirectMask;
	XSendEvent(bbtool->getXDisplay(), 
						 bbtool->getCurrentScreenInfo()->getRootWindow(),
						 False, mask, &e);
	bbtool->moveWinToDesktop(win, desk_number);
}

void WMInterface::decorateToggleWindow(Window win)
{
	XEvent e;
	e.xclient.type = ClientMessage;
	e.xclient.message_type = bbtool->getBlackboxHintsAtom();
	e.xclient.window = win;
	e.xclient.format = 32;
	e.xclient.data.l[0] = e.xclient.data.l[1] =
	e.xclient.data.l[2] = e.xclient.data.l[3] = 
	e.xclient.data.l[4] = e.xclient.data.l[5] = 0l;
	e.xclient.data.l[0] |= AttribDecoration;

	int format;
	Atom atom_return;
	unsigned long num, len;
	BlackboxHints *net_attributes;
						
	if (XGetWindowProperty(bbtool->getXDisplay(), win,
			bbtool->getBlackboxHintsAtom(), 0,
			PropBlackboxHintsElements, False,
			bbtool->getBlackboxHintsAtom(), &atom_return,
			&format, &num, &len, (unsigned char **) &net_attributes)
			==
			Success && net_attributes) {
		if(num == PropBlackboxHintsElements) {
			if (net_attributes->decoration == DecorNone)
				e.xclient.data.l[4] = DecorNormal;
			else
				e.xclient.data.l[4] = DecorNone;
		}
		XFree((void *) net_attributes);
	} else
		e.xclient.data.l[4] = DecorNone;

	XSendEvent(bbtool->getXDisplay(), bbtool->getScreenInfo(0)->getRootWindow(), False,
			SubstructureRedirectMask, &e);
}

void WMInterface::setWindowFocus(Window win) {
	XEvent e;
	unsigned long mask;

	e.xclient.type = ClientMessage;
	e.xclient.window =	win;

	e.xclient.message_type = bbtool->getBlackboxChangeWindowFocusAtom();
	e.xclient.format = 32;
	e.xclient.data.l[0] = 0;
	e.xclient.data.l[1] = 0;

	mask =	SubstructureRedirectMask;
	XSendEvent(bbtool->getXDisplay(), 
						 bbtool->getCurrentScreenInfo()->getRootWindow(),
						 False, mask, &e);
}

void WMInterface::shadeWindow(Window win) {
	XEvent e;
	e.xclient.type = ClientMessage;
	e.xclient.message_type = bbtool->getBlackboxChangeAttributesAtom();
	e.xclient.window = win;
	e.xclient.format = 32;
	e.xclient.data.l[0] = AttribShaded;
	e.xclient.data.l[2] = e.xclient.data.l[3] = e.xclient.data.l[4] = 0l;

	Atom atom_return;
	int foo;
	unsigned long ulfoo, nitems;
	BlackboxAttributes *net;
	if (XGetWindowProperty(bbtool->getXDisplay(), win,
			bbtool->getBlackboxAttributesAtom(),
			0l,
			PropBlackboxAttributesElements,
			False,
			bbtool->getBlackboxAttributesAtom(),
			&atom_return, &foo, &nitems, &ulfoo, (unsigned char **) &net)
			==
			Success && net && nitems
			==
			PropBlackboxAttributesElements)
	{
		e.xclient.data.l[1] = net->attrib ^ AttribShaded;
		XFree((void *) net);
	} else
		e.xclient.data.l[1] = AttribShaded;

	XSendEvent(bbtool->getXDisplay(), bbtool->getScreenInfo(0)->getRootWindow(), False,
					SubstructureRedirectMask, &e);
}

void WMInterface::maximizeWindow(Window win, bool maxhorz, bool maxvert) {
	XEvent e;
	e.xclient.type = ClientMessage;
	e.xclient.message_type = bbtool->getBlackboxChangeAttributesAtom();
	e.xclient.window = win;
	e.xclient.format = 32;
	e.xclient.data.l[0] = (maxhorz?AttribMaxHoriz:0) | (maxvert?AttribMaxVert:0);
	e.xclient.data.l[2] = e.xclient.data.l[3] = e.xclient.data.l[4] = 0l;

	Atom atom_return;
	int foo;
	unsigned long ulfoo, nitems;
	BlackboxAttributes *net;
	if (XGetWindowProperty(bbtool->getXDisplay(),
			win,
			bbtool->getBlackboxAttributesAtom(),
			0l,
			PropBlackboxAttributesElements,
			False,
			bbtool->getBlackboxAttributesAtom(),
			&atom_return, &foo, &nitems, &ulfoo, (unsigned char **) &net)
			==
			Success && net && nitems
			==
			PropBlackboxAttributesElements) {
		e.xclient.data.l[1] =
				net->attrib ^ ((maxhorz?AttribMaxHoriz:0) | (maxvert?AttribMaxVert:0));
		XFree((void *) net);
	} else
		e.xclient.data.l[1] = (maxhorz?AttribMaxHoriz:0) | (maxvert?AttribMaxVert:0);

	XSendEvent(bbtool->getXDisplay(), bbtool->getScreenInfo(0)->getRootWindow(),
			False, SubstructureRedirectMask, &e);
}

void WMInterface::stickWindow(Window win)
{
	XEvent e;
	e.xclient.type = ClientMessage;
	e.xclient.message_type = bbtool->getBlackboxChangeAttributesAtom();
	e.xclient.window = win;
	e.xclient.format = 32;
	e.xclient.data.l[0] = AttribOmnipresent;
	e.xclient.data.l[2] = e.xclient.data.l[3] = e.xclient.data.l[4] = 0l;

	Atom atom_return;
	int foo;
	unsigned long ulfoo, nitems;
	BlackboxAttributes *net;
	if (XGetWindowProperty(bbtool->getXDisplay(),
			win,
			bbtool->getBlackboxAttributesAtom(),
			0l,
			PropBlackboxAttributesElements,
			False,
			bbtool->getBlackboxAttributesAtom(),
			&atom_return, &foo, &nitems, &ulfoo, (unsigned char **) &net)
			==
			Success && net && nitems
			==
			PropBlackboxAttributesElements) {
		e.xclient.data.l[1] = net->attrib ^ AttribOmnipresent;
		XFree((void *) net);
	} else
		e.xclient.data.l[1] = AttribOmnipresent;

	XSendEvent(bbtool->getXDisplay(), bbtool->getScreenInfo(0)->getRootWindow(), False,
			SubstructureRedirectMask, &e);
}

int WMInterface::isIconicState(Window win) {
	Atom real_type;
	int format;
	unsigned long n, extra;
	int status;
	long *p=0;
	int y=0;

	status = XGetWindowProperty(bbtool->getXDisplay(), win, 
															bbtool->getWMStateAtom(), 0L, 1L,
															False, bbtool->getWMStateAtom(), &real_type,
															&format, &n, &extra,	(unsigned char**)&p);
	if (!status) {
		if (p) {
			 y=(int)p[0];
			 XFree(p);
			 return (y==IconicState) ? 1 : 0;
		} else {
			 return 0;
		}
	}

	return -1;
}

int WMInterface::getAttributes(Window win) {
	Atom real_type;
	int format;
	unsigned long n, extra;
	int status;
	long *p=0;
	int y=0;

	
	status = XGetWindowProperty(bbtool->getXDisplay(), win, 
															bbtool->getBlackboxAttributesAtom(), 0L, 1L,
															False, bbtool->getBlackboxAttributesAtom(), 
															&real_type, &format,&n, &extra,	 
															(unsigned char**)&p);
	if (!status) {
		if (p) {
			 y=(int)p[0];
			 XFree(p);
			 return y;
		} else {
			 return 0;
		}
	}

	return -1;
}

void WMInterface::NETNotifyStartup() {
//	bbtool->setNETInit();
}

void WMInterface::NETNotifyWindowAdd(Window win,int desktop_nr) {
	bbtool->addWindow(win, desktop_nr);
}

void WMInterface::NETNotifyDel(Window win) {
	bbtool->removeWindow(win);
}

void WMInterface::NETNotifyAttributes(Window win)
{
	bbtool->windowAttributeChange(win);
}

void WMInterface::NETNotifyFocus(Window win) {
	if (win)
		if (focusWindow)
			bbtool->focusWindow(win);
		else {
			setWindowFocus(bbtool->getFocusWindow());
			focusWindow = True;
		}
}

void WMInterface::NETNotifyCurrentWorkspace(int desktop_nr) {
	bbtool->focusDesktop(desktop_nr);
}

void WMInterface::NETNotifyWorkspaceCount(int count) {
	bbtool->setDesktopCount(count);
}
