//	wminterface.hh for bbtools.
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
// $Id: wminterface.hh,v 1.4 2002/05/30 07:30:03 eckzor Exp $

#ifndef __WMINTERFACE_HH
#define __WMINTERFACE_HH

#include "NETInterface.hh"

class ToolWindow;
struct WindowList;

class WMInterface : public NETInterface {

public:
	WMInterface(ToolWindow *);
	~WMInterface(void);
	void sendWindowToDesktop(Window,int);
	void shadeWindow(Window);
	void maximizeWindow(Window,bool,bool);
	void decorateToggleWindow(Window);
	void stickWindow(Window);
	void setWindowFocus(Window);
	int isIconicState(Window);
	int getAttributes(Window);
	void sendClientMessage(Atom, XID);
	void changeDesktop(int,bool focusWin=True);

protected:
	virtual void NETNotifyStartup(void);
	virtual void NETNotifyWindowAdd(Window,int);
	virtual void NETNotifyDel(Window);
	virtual void NETNotifyAttributes(Window);
	virtual void NETNotifyFocus(Window);
	virtual void NETNotifyCurrentWorkspace(int);
	virtual void NETNotifyWorkspaceCount(int);

private:
	ToolWindow *bbtool;
};

#endif /* __WMINTERFACE_HH */
