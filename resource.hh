// resource.hh for bbkeys - a pager tool for Blackbox
//
//  Copyright (c) 1998-1999 John Kennis, jkennis@chello.nl
//
//  this program is free software; you can redistribute it and/or modify
//  it under the terms of the gnu general public license as published by
//  the free software foundation; either version 2 of the license, or
//  (at your option) any later version.
//
//  this program is distributed in the hope that it will be useful,
//  but without any warranty; without even the implied warranty of
//  merchantability or fitness for a particular purpose.  see the
//  gnu general public license for more details.
//
//  you should have received a copy of the gnu general public license
//  along with this program; if not, write to the free software
//  foundation, inc., 675 mass ave, cambridge, ma 02139, usa.
//
// (see the included file copying / gpl-2.0)
//


#ifndef __RESOURCE_HH
#define __RESOURCE_HH

#include "bbkeys.hh"
#include "Baseresource.hh"

class BaseResource;

enum {none=0, border=1, texture=2};
enum WHICH_BUTTON { LEFT_BUTTON = 1, MIDDLE_BUTTON, RIGHT_BUTTON,
                    FOURTH_BUTTON, FIFTH_BUTTON};

struct FRAME {
  int width;
  int height;
  BTexture texture;
  int bevelWidth;
  XFontStruct *font;
};

struct POSITION {
  int x;
  int y;
  int mask;
  bool vertical;
  bool horizontal;
};

struct LABEL {
    int width;
    int width0, width1, width2, width3;
    int height;
    bool transparent;
    BTexture texture;
    XFontStruct *font;
    BColor textColor;
};

struct SHOW {
    bool newmail_counter;
    bool totalmail_counter;
    bool label;
    bool envelope;
    bool onlyAtNewMail;
};

struct SIZE {
  unsigned int width;
  unsigned int height;
};

struct BBPAGERWIN {
  BTexture texture;
  BTexture focusedTexture;
  BColor activeColor;
  BColor inactiveColor;
};

struct BUTTON {
    BTexture texture;
    BTexture texture_pressed;
};

struct MENU {
	Bool stackedCycling;
	Bool showCycleMenu;
	Bool showAll;
	BTexture texture;
	BTexture hiTexture;
	BColor textColor;
	BColor hiTextColor;
	BColor highlightColor;
	int justify;
	XFontStruct *font;
  int bullet_style;
  int bullet_pos;
};

class Resource : public BaseResource {

public:
  Resource(ToolWindow *);
  ~Resource(void);

  struct FRAME frame;
  struct LABEL label;
  struct SHOW show;
  struct BUTTON button;
  struct POSITION position;
  struct SIZE desktopSize;
  struct MENU menu;
  struct BBPAGERWIN pagerwin;
  struct BBPAGERWIN desktopwin;
  int columns;
  int rows;
	Bool getMenuStackedCycling(void) { return menu.stackedCycling; }
	Bool getMenuShowCycleMenu(void) { return menu.showCycleMenu; }
	Bool getMenuShowAllWorkspaces(void) { return menu.showAll; }
  int getFocusStyle(void) { return focus_style; }
  int getDesktopFocusStyle(void) { return desktop_focus_style; }
  WHICH_BUTTON getWindowMoveButton(void) { return window_move_button; }
  WHICH_BUTTON getDesktopChangeButton(void) { return desktop_change_button; }
  WHICH_BUTTON getWindowRaiseButton(void) { return window_raise_button; }
  WHICH_BUTTON getWindowFocusButton(void) { return window_focus_button; }

  /* menu */
 	const int getJustification(void) const {return menu.justify; }
	int getMenuJustification(void) { return menu.justify; }	
	XFontStruct *getTitleFont(void) { return menu.font; }
	XFontStruct *getMenuFont(void) { return menu.font; }
	unsigned int getBevelWidth(void) { return frame.bevelWidth; }
  BColor *getBorderColor(void) { return frame.texture.getColor(); }
  unsigned int getBorderWidth(void) { return 0; }
  int getBulletStyle(void) { return menu.bullet_style; }
  int getBulletPosition(void) { return menu.bullet_pos; }

  enum { AlignDontCare = 1, AlignTop, AlignBottom };
  enum { Right = 1, Left };
  enum { Empty = 0, Round, Square, Triangle, Diamond };
  enum { LeftJustify = 1, RightJustify, CenterJustify };

protected:
  virtual void  LoadBBToolResource(void);

private:
  void Frame(void);
  void SizeAndPosition(void);
  void PagerWin(void);
  int focus_style;
  int desktop_focus_style;
  void Label(void);
  void Show(void);
  void Button(void);
	void Menu(void);
  WHICH_BUTTON window_move_button;
  WHICH_BUTTON desktop_change_button;
  WHICH_BUTTON window_raise_button;
  WHICH_BUTTON window_focus_button;
  void Clean(void);
};
#endif /* __RESOURCE_HH */
