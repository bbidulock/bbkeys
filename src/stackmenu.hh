// stackmenu.hh for bbkeys
//
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
// $Id: stackmenu.hh,v 1.5 2002/05/30 20:20:45 eckzor Exp $

#ifndef __STACKMENU_HH
#define __STACKMENU_HH

#include "Basemenu.hh"

class ToolWindow;

class Stackmenu : public Basemenu {
public:
	Stackmenu(ToolWindow *);
	virtual ~Stackmenu();

	void hide();
	void show();
	void show(bool forward, bool showMenu);
	void reconfigure();
	void key_press(int grabInt);
	void centerPosition();
	void setSelected(int);
	void setMenuItems();
	void selectFocused(bool);

protected:
	void clearMenu();
	void itemSelected(int,int);

private:
	ToolWindow *bbtool;
	int menuPosition;
};

#endif // __STACKMENU_HH
