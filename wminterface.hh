//	wminterface.hh for bbtools.
//
//	Copyright (c) 1998-1999 by John Kennis, jkennis@chello.nl
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
	void changeDesktop(int);

/*	Atom getKWMModuleDesktopChange(void) { return kwm_module_desktop_change; }
	Atom getKWMModuleDesktopNumberChange(void) {
								return kwm_module_desktop_number_change; }
	Atom getKWMModuleInit(void) { return kwm_module_init; }
	Atom getKWMModuleWinAdd(void) { return kwm_module_win_add; }
	Atom getKWMModuleWinRemove(void) { return kwm_module_win_remove; }
	Atom getKWMModuleWinChange(void) { return kwm_module_win_change; }
	Atom getKWMModuleWinActivate(void) { return kwm_module_win_activate; }
	Atom getKWMModuleWinRaise(void) { return kwm_module_win_raise; }
	Atom getKWMModuleWinLower(void) { return kwm_module_win_lower; }*/
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

/*	Atom kwm_module_init;
	Atom kwm_module_win_add;
	Atom kwm_module_win_remove;
	Atom kwm_module_win_change;
	Atom kwm_module_win_activate;
	Atom kwm_module_win_iconfied;
	Atom kwm_module_win_raise;
	Atom kwm_module_win_lower;
	Atom kwm_module_desktop_change;
	Atom kwm_module_desktop_number_change;*/

};

#endif /* __WMINTERFACE_HH */
