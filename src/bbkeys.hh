// bbkeys.hh for bbkeys - Ye Olde Keystroke Grabber for Blackbox
//
//	Copyright (c) 1998-1999 by John Kennis, jkennis@chello.nl
//	Copyright (c) 1999-2001 by Jason Kasper (vanRijn) vR@movingparts.net
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
// $Id: bbkeys.hh,v 1.12 2002/07/19 06:45:25 eckzor Exp $

#ifndef __BBKEYS_HH
#define __BBKEYS_HH

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "grab_defs.hh" // grab* definitions
#include "Basewindow.hh"
#include "Timer.hh"

class Stackmenu;
class Resource;
class WMInterface;
class Basemenu;

#define MaxInstructions 100
#define RCFILE ".bbkeysrc"

typedef struct {
  unsigned long modMask;
  KeyCode keycode;
  int action;
  char *execCommand;
} KEY_ACTION_MAP;

typedef struct {
  int instructCount;
  KEY_ACTION_MAP KeyMap[MaxInstructions];
} KEY_GRAB_INSTRUCTION;

struct PIXMAP {
	Pixmap	frame;
	Pixmap	window;
//	Pixmap	focusedDesktop;
//	Pixmap	focusedWindow;
//	Pixmap	desktop;

	Pixmap	pix_title;
	Pixmap	pix_back;
	Pixmap	pix_closeBtn;
	Pixmap	pix_configBtn;
	Pixmap	pix_pressedBtn;
};

struct GEOM {
	int height;
	int width;
	int x;
	int y;
};

struct WindowList {
	Window win;
	bool iconic;
	bool shaded;
	bool sticky;
	int desktop;
};

struct DesktopList {
	int number;
};

class Stackmenu;

class ToolWindow : public Basewindow, public TimeoutHandler {
public:
	ToolWindow(int argc,char **argv,struct CMDOPTIONS *);
	~ToolWindow(void);

	void reconfigure(void);
	void execCommand(char *const);

	// functions for translation of grab information
	const char *index_to_name(int);
	int translateAction(char *);
	int translateModifier(char *);

	// functions for handling/grabbing keys
	void loadKeygrabs(void);
	void setKeygrabs(void);
	void activateKeygrabs(void);
	void getOffendingModifiers(void);
	void wHackedGrabKey(int,unsigned int,Window,Bool,int,int);
	void x_reset_modifier_mapping(Display *);
	void InitializeModifiers(void);
 
	// functions for maintaining the desktop list
	void addDesktop(void);
	void removeDesktop(int);
	void focusDesktop(int);
	void setDesktopCount(int);	// this should only be called during the
															// initialization stage
	
	// functions for maintaining the window list
	void addWindow(Window,int);
	void removeWindow(Window);
	void clearWindows(void);
	void focusWindow(Window);
	void moveWinToDesktop(Window,int);
	void windowAttributeChange(Window); // called when attributes for a window
																			// have changed, such as sticky
	void cycleWindowFocus(bool);

	// functions for modifying windows
	void raiseWindow(Window);
	void lowerWindow(Window);

	// functions for the Linear window cycling style
	void add_linear(WindowList *);
	void cycle_linear(bool);

	// functions for the Stack window cycling style
	void add_stack(WindowList *);
	void cycle_stack(bool);
	void focus_stack(Window);
	void saveMenuSearch(Window,Basemenu *);
	void removeMenuSearch(Window);
	
	// functions which return information
	inline int getCurrentDesktopNr(void) {
		return current_desktop?current_desktop->number:-1;
	}
	inline Window getFocusWindow() { return focus_window; }
	inline int getDesktopCount(void) { return desktop_count; }
	inline Resource *getResource() { return resource; }
	inline GC getMenuTitleGC(void) {return NULL;/*menuTitleGC;*/}
	inline GC getMenuHiBGGC(void) {return menuHiBGGC;}
	inline GC getMenuHiGC(void) {return menuHiGC; }
	inline GC getMenuFrameGC(void) {return menuFrameGC;}
	inline bool getDoingCycling(void) { return doingCycling; }
	inline bool getShowAllWorkspaces(void) { return showAllWorkspaces; }
	unsigned int KeycodeToModmask(unsigned int code);

//	void setNETInit(void) { /*wm_init = True;*/ }

protected:
	// functions for dealing with the bbkeys window
	void MakeWindow(bool);
	void Redraw(void);
	virtual void process_event(XEvent *);
	virtual void CheckConfig(void);
	void timeout(void);
	void addSticky(WindowList *);
	void removeSticky(const Window,const int);
	inline void setCycling(bool f) { doingCycling = f; }
	inline void setShowAllWorkspaces(bool f) { showAllWorkspaces = f; }
  void findFramePosition(Window window, int &x, int &y);
	
private:
	XGCValues gcv;	// font stuff
	GC frameGC;

	Window win_frame; // windows for the tool
	Window win_title;
	Window win_back;
	Window win_closeBtn;
	Window win_configBtn;
	GEOM frame;				// size/pos data for the windows and label
	GEOM label;
	GEOM geom_title;
	GEOM geom_back;
	GEOM geom_closeBtn;
	GEOM geom_configBtn;
	PIXMAP	pixmap;		// pixmaps for all of the windows
	BTimer *timer;		// window background drawing timer
//	bool raised;			// window is raised above all others

	KEY_GRAB_INSTRUCTION grabSet; // key grab/handling vars
	int actionList[NUM_GRABS];
	unsigned int ValidModMask;
	unsigned int _NumLockMask;
	unsigned int _ScrollLockMask;

	char *bbkeys_rcfile;  // resource file
	time_t bbkeys_rcTime; // time used for autoloading resource file
	Resource *resource;   // resource data

	int desktop_count;										// the number of desktops
	DesktopList *current_desktop;					// the currently active desktop
	LinkedList<WindowList> *windowList;		// list of all windows
	LinkedList<DesktopList> *desktopList; // list of all desktop windows
	Window focus_window;									// window which has the focus

	WMInterface *wminterface; // interface for communicating with the
														// window manager

	Stackmenu *stackMenu;		// variables for the stack cycling style menu
	bool doingCycling;			// are we in the middle of a stacked cycle?
	bool showAllWorkspaces;
	Window menuWin;
	GC menuGC;
	GC menuHiBGGC;
	GC menuHiGC;
	GC menuFrameGC;

	friend class Stackmenu;
	void p();
};

#endif /* __BBKEYS_HH */
