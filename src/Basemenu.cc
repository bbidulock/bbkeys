// Basemenu.cc for Blackbox - an X11 Window manager
// Copyright (c) 1997 - 2000 Brad Hughes (bhughes@tcac.net)
// Copyright (c) 2001 by Ben Jansens <xor@orodu.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

// stupid macros needed to access some functions in version 2 of the GNU C
// library
#ifndef   _GNU_SOURCE
#define   _GNU_SOURCE
#endif // _GNU_SOURCE

#ifdef    HAVE_CONFIG_H
#  include "../config.h"
#endif // HAVE_CONFIG_H

#include "bbkeys.hh"
#include "Basemenu.hh"

#ifdef    HAVE_STDIO_H
#  include <stdio.h>
#endif // HAVE_STDIO_H

#ifdef    STDC_HEADERS
#  include <stdlib.h>
#  include <string.h>
#endif // STDC_HEADERS


static Basemenu *shown = (Basemenu *) 0;

char *bstrdup(const char *s) {
	int l = strlen(s) + 1;
	char *n = new char[l];
	strncpy(n, s, l);
	return n;
}

Basemenu::Basemenu(ToolWindow *toolwindow) {
  bbtool = toolwindow;
  image_ctrl = bbtool->getImageControl();
  display = bbtool->getXDisplay();
  parent = (Basemenu *) 0;
  alignment = AlignDontCare;

  title_vis =
    movable =
    hide_tree = True;

  shifted =
    internal_menu =
    moving =
    torn =
    visible = False;

  menu.x =
    menu.y =
    menu.x_shift =
    menu.y_shift =
    menu.x_move =
    menu.y_move = 0;

  which_sub =
    which_press =
    which_sbl = -1;

  menu.frame_pixmap =
    menu.title_pixmap =
    menu.hilite_pixmap =
    menu.sel_pixmap = None;

  menu.bevel_w = bbtool->getResource()->getBevelWidth();

  menu.width = menu.title_h = menu.item_w = menu.frame_h =
	 bbtool->getResource()->getMenuFont()->ascent +
	 bbtool->getResource()->getMenuFont()->descent +
	 (menu.bevel_w * 2);
  
  menu.label = 0;
  
  menu.sublevels =
    menu.persub =
    menu.minsub = 0;
  
  menu.item_h = bbtool->getResource()->getMenuFont()->ascent +
	 bbtool->getResource()->getMenuFont()->descent +
	 (menu.bevel_w);
  
  menu.height = menu.title_h + bbtool->getResource()->getBorderWidth() + menu.frame_h;
  
  unsigned long attrib_mask = CWBackPixmap | CWBackPixel | CWBorderPixel |
			      /*CWColormap | */CWOverrideRedirect | CWEventMask;
  XSetWindowAttributes attrib;
  attrib.background_pixmap = None;
  attrib.background_pixel = attrib.border_pixel =
			    bbtool->getResource()->getBorderColor()->getPixel();
//  attrib.colormap = bbtool->getCurrentScreenInfo()->getColormap();
  attrib.override_redirect = True;
  attrib.event_mask = ButtonPressMask | ButtonReleaseMask |
                      ButtonMotionMask | ExposureMask;

  menu.window =
    XCreateWindow(display, bbtool->getCurrentScreenInfo()->getRootWindow(), menu.x, menu.y, menu.width,
		  menu.height, bbtool->getResource()->getBorderWidth(), bbtool->getCurrentScreenInfo()->getDepth(),
                  InputOutput, bbtool->getCurrentScreenInfo()->getVisual(), attrib_mask, &attrib);
  bbtool->saveMenuSearch(menu.window, this);

  attrib_mask = CWBackPixmap | CWBackPixel | CWBorderPixel | CWEventMask;
  attrib.background_pixel = bbtool->getResource()->getBorderColor()->getPixel();
  attrib.event_mask |= EnterWindowMask | LeaveWindowMask;

  menu.title =
    XCreateWindow(display, menu.window, 0, 0, menu.width, menu.height, 0,
		  bbtool->getCurrentScreenInfo()->getDepth(),
			InputOutput, bbtool->getCurrentScreenInfo()->getVisual(),
		  attrib_mask, &attrib);
  bbtool->saveMenuSearch(menu.title, this);

  attrib.event_mask |= PointerMotionMask;
  menu.frame = XCreateWindow(display, menu.window, 0,
			     menu.title_h + bbtool->getResource()->getBorderWidth(),
			     menu.width, menu.frame_h, 0,
			     bbtool->getCurrentScreenInfo()->getDepth(), InputOutput,
			     bbtool->getCurrentScreenInfo()->getVisual(), attrib_mask, &attrib);
  bbtool->saveMenuSearch(menu.frame, this);

  menuitems = new LinkedList<BasemenuItem>;

  // even though this is the end of the constructor the menu is still not
  // completely created.  items must be inserted and it must be update()'d
}


Basemenu::~Basemenu(void) {
  XUnmapWindow(display, menu.window);

  if (shown && shown->getWindowID() == getWindowID())
    shown = (Basemenu *) 0;

  int n = menuitems->count();
  for (int i = 0; i < n; ++i)
    remove(0);

  delete menuitems;

  if (menu.label)
    delete [] menu.label;

  if (menu.title_pixmap)
    image_ctrl->removeImage(menu.title_pixmap);

  if (menu.frame_pixmap)
    image_ctrl->removeImage(menu.frame_pixmap);

  if (menu.hilite_pixmap)
    image_ctrl->removeImage(menu.hilite_pixmap);

  if (menu.sel_pixmap)
    image_ctrl->removeImage(menu.sel_pixmap);

  bbtool->removeMenuSearch(menu.title);
  XDestroyWindow(display, menu.title);

  bbtool->removeMenuSearch(menu.frame);
  XDestroyWindow(display, menu.frame);

  bbtool->removeMenuSearch(menu.window);
  XDestroyWindow(display, menu.window);
}


int Basemenu::insert(const char *l, int function, const char *e, int pos) {
  char *label = 0, *exec = 0;

  if (l) label = bstrdup(l);
  if (e) exec = bstrdup(e);

  BasemenuItem *item = new BasemenuItem(label, function, exec);
  menuitems->insert(item, pos);

  return menuitems->count();
}


int Basemenu::insert(const char *l, Basemenu *submenu, int pos) {
  char *label = 0;

  if (l) label = bstrdup(l);

  BasemenuItem *item = new BasemenuItem(label, submenu);
  menuitems->insert(item, pos);

  submenu->parent = this;

  return menuitems->count();
}


int Basemenu::insert(const char **ulabel, int pos, int function) {
  BasemenuItem *item = new BasemenuItem(ulabel, function);
  menuitems->insert(item, pos);

  return menuitems->count();
}


int Basemenu::remove(int index) {
  if (index < 0 || index > menuitems->count()) return -1;

  BasemenuItem *item = menuitems->remove(index);

  if (item) {
    if ((! internal_menu) && (item->submenu())) {
      Basemenu *tmp = (Basemenu *) item->submenu();

      if (! tmp->internal_menu) {
	delete tmp;
      } else
	tmp->internal_hide();
    }

    if (item->label())
      delete [] item->label();
    
    if (item->exec())
      delete [] item->exec();

    delete item;
  }

  if (which_sub == index)
    which_sub = -1;
  else if (which_sub > index)
    which_sub--;

  return menuitems->count();
}


void Basemenu::update(void) {
  menu.item_h = bbtool->getResource()->getMenuFont()->ascent +
	  bbtool->getResource()->getMenuFont()->descent +
	  menu.bevel_w;
  menu.title_h = bbtool->getResource()->getMenuFont()->ascent +
	   bbtool->getResource()->getMenuFont()->descent +
	   (menu.bevel_w * 2);
    
  if (title_vis) {
    menu.item_w = XTextWidth(bbtool->getResource()->getTitleFont(),
			((menu.label) ? menu.label : "Blackbox Menu"),
			strlen((menu.label) ? menu.label : "Blackbox Menu"));
    
    menu.item_w += (menu.bevel_w * 2);
  }  else
    menu.item_w = 1;

  int ii = 0;
  LinkedListIterator<BasemenuItem> it(menuitems);
  for (; it.current(); it++) {
    BasemenuItem *itmp = it.current();

    const char *s = ((itmp->u && *itmp->u) ? *itmp->u :
		     ((itmp->l) ? itmp->l : (const char *) 0));
    int l = strlen(s);

    ii = XTextWidth(bbtool->getResource()->getMenuFont(), s, l);

    ii += (menu.bevel_w * 2) + (menu.item_h * 2);

    menu.item_w = ((menu.item_w < (unsigned int) ii) ? ii : menu.item_w);
  }

  if (menuitems->count()) {
    menu.sublevels = 1;

    while (((menu.item_h * (menuitems->count() + 1) / menu.sublevels)
	    + menu.title_h + bbtool->getResource()->getBorderWidth()) >
	   bbtool->getCurrentScreenInfo()->getHeight())
      menu.sublevels++;

    if (menu.sublevels < menu.minsub) menu.sublevels = menu.minsub;

    menu.persub = menuitems->count() / menu.sublevels;
    if (menuitems->count() % menu.sublevels) menu.persub++;
  } else {
    menu.sublevels = 0;
    menu.persub = 0;
  }

  menu.width = (menu.sublevels * (menu.item_w));
  if (! menu.width) menu.width = menu.item_w;

  menu.frame_h = (menu.item_h * menu.persub);
  menu.height = ((title_vis) ? menu.title_h + bbtool->getResource()->getBorderWidth() : 0) +
		menu.frame_h;
  if (! menu.frame_h) menu.frame_h = 1;
  if (menu.height < 1) menu.height = 1;

  Pixmap tmp;
  BTexture *texture;
  if (title_vis) {
    tmp = menu.title_pixmap;
    texture = &(bbtool->getResource()->menu.texture);
    if (texture->getTexture() == (BImage_Flat | BImage_Solid)) {
      menu.title_pixmap = None;
      XSetWindowBackground(display, menu.title,
			   texture->getColor()->getPixel());
    } else {
      menu.title_pixmap =
        image_ctrl->renderImage(menu.width, menu.title_h, texture);
      XSetWindowBackgroundPixmap(display, menu.title, menu.title_pixmap);
    }
    if (tmp) image_ctrl->removeImage(tmp);
    XClearWindow(display, menu.title);
  }

  tmp = menu.frame_pixmap;
  texture = &(bbtool->getResource()->menu.texture);
  if (texture->getTexture() == (BImage_Flat | BImage_Solid)) {
    menu.frame_pixmap = None;
    XSetWindowBackground(display, menu.frame,
			 texture->getColor()->getPixel());
  } else {
    menu.frame_pixmap =
      image_ctrl->renderImage(menu.width, menu.frame_h, texture);
    XSetWindowBackgroundPixmap(display, menu.frame, menu.frame_pixmap);
  }
  if (tmp) image_ctrl->removeImage(tmp);

  tmp = menu.hilite_pixmap;
  texture = &(bbtool->getResource()->menu.hiTexture);
  if (texture->getTexture() == (BImage_Flat | BImage_Solid))
    menu.hilite_pixmap = None;
  else
    menu.hilite_pixmap =
      image_ctrl->renderImage(menu.item_w, menu.item_h, texture);
  if (tmp) image_ctrl->removeImage(tmp);

  tmp = menu.sel_pixmap;
  if (texture->getTexture() == (BImage_Flat | BImage_Solid))
    menu.sel_pixmap = None;
  else {
    int hw = menu.item_h / 2;
    menu.sel_pixmap =
      image_ctrl->renderImage(hw, hw, texture);
  }
  if (tmp) image_ctrl->removeImage(tmp);

  XResizeWindow(display, menu.window, menu.width, menu.height);

  if (title_vis)
    XResizeWindow(display, menu.title, menu.width, menu.title_h);

  XMoveResizeWindow(display, menu.frame, 0,
		    ((title_vis) ? menu.title_h +
		     bbtool->getResource()->getBorderWidth() : 0), menu.width,
		    menu.frame_h);

  XClearWindow(display, menu.window);
  XClearWindow(display, menu.title);
  XClearWindow(display, menu.frame);

  if (title_vis && visible) redrawTitle();

  int i = 0;
  for (i = 0; visible && i < menuitems->count(); i++)
    if (i == which_sub) {
      drawItem(i, True, 0);
      drawSubmenu(i);
    } else
      drawItem(i, (i==selected_item?True:False), 0);

  if (parent && visible)
    parent->drawSubmenu(parent->which_sub);

  XMapSubwindows(display, menu.window);
}


void Basemenu::show(void) {
  XMapSubwindows(display, menu.window);
  XMapWindow(display, menu.window);
  visible = True;

  if (! parent) {
    if (shown && (! shown->torn))
       shown->hide();

    shown = this;
  }
}


void Basemenu::hide(void) {
  if ((! torn) && hide_tree && parent && parent->isVisible()) {
    Basemenu *p = parent;

    while (p->isVisible() && (! p->torn) && p->parent) p = p->parent;
    p->internal_hide();
  } else
    internal_hide();
}


void Basemenu::internal_hide(void) {
  if (which_sub != -1) {
    BasemenuItem *tmp = menuitems->find(which_sub);
    tmp->submenu()->internal_hide();
  }

  if (parent && (! torn)) {
    parent->drawItem(parent->which_sub, False, True);

    parent->which_sub = -1;
  } else if (shown && shown->menu.window == menu.window)
    shown = (Basemenu *) 0;

  torn = visible = False;
  which_sub = which_press = which_sub = -1;

  XUnmapWindow(display, menu.window);
}


void Basemenu::move(int x, int y) {
  menu.x = x;
  menu.y = y;
  XMoveWindow(display, menu.window, x, y);
  if (which_sub != -1)
    drawSubmenu(which_sub);
}


void Basemenu::redrawTitle(void) {
  int dx = menu.bevel_w;
  unsigned int l;

  l = XTextWidth(bbtool->getResource()->getTitleFont(),
			((menu.label) ? menu.label : "Blackbox Menu"),
			strlen(((menu.label) ? menu.label : "Blackbox Menu")));

  l +=  (menu.bevel_w * 2);

  switch (bbtool->getResource()->getJustification()) {
  case RightJustify:
    dx += menu.width - l;
    break;

  case CenterJustify:
    dx += (menu.width - l) / 2;
    break;
  }

  XDrawString(display, menu.title, bbtool->getMenuTitleGC(), dx,
		bbtool->getResource()->getTitleFont()->ascent + menu.bevel_w,
		((menu.label) ? menu.label : "Blackbox Menu"),
		strlen(((menu.label) ? menu.label : "Blackbox Menu")));
}


void Basemenu::drawSubmenu(int index) {
  if (which_sub != -1 && which_sub != index) {
    BasemenuItem *itmp = menuitems->find(which_sub);

    if (! itmp->submenu()->isTorn())
      itmp->submenu()->internal_hide();
  }

  if (index >= 0 && index < menuitems->count()) {
    BasemenuItem *item = menuitems->find(index);
    if (item->submenu() && visible && (! item->submenu()->isTorn()) &&
	item->isEnabled()) {
      if (item->submenu()->parent != this) item->submenu()->parent = this;
      int sbl = index / menu.persub, i = index - (sbl * menu.persub),
	    x = menu.x +
		((menu.item_w * (sbl + 1)) + bbtool->getResource()->getBorderWidth()), y;
    
      if (alignment == AlignTop)
	y = (((shifted) ? menu.y_shift : menu.y) +
	     ((title_vis) ? menu.title_h + bbtool->getResource()->getBorderWidth() : 0) -
	     ((item->submenu()->title_vis) ?
	      item->submenu()->menu.title_h + bbtool->getResource()->getBorderWidth() : 0));
      else
	y = (((shifted) ? menu.y_shift : menu.y) +
	     (menu.item_h * i) +
	     ((title_vis) ? menu.title_h + bbtool->getResource()->getBorderWidth() : 0) -
	     ((item->submenu()->title_vis) ?
	      item->submenu()->menu.title_h + bbtool->getResource()->getBorderWidth() : 0));

      if (alignment == AlignBottom &&
	  (y + item->submenu()->menu.height) > ((shifted) ? menu.y_shift :
						menu.y) + menu.height)
	y = (((shifted) ? menu.y_shift : menu.y) +
	     menu.height - item->submenu()->menu.height);

      if ((x + item->submenu()->getWidth()) > bbtool->getCurrentScreenInfo()->getWidth()) {
	x = ((shifted) ? menu.x_shift : menu.x) -
	    item->submenu()->getWidth() - bbtool->getResource()->getBorderWidth();
      }
      
      if (x < 0) x = 0;

      if ((y + item->submenu()->getHeight()) > bbtool->getCurrentScreenInfo()->getHeight())
	y = bbtool->getCurrentScreenInfo()->getHeight() - item->submenu()->getHeight() -
	    bbtool->getResource()->getBorderWidth();
      if (y < 0) y = 0;
      
      item->submenu()->move(x, y);
      if (! moving) drawItem(index, True);
    
      if (! item->submenu()->isVisible())
	item->submenu()->show();
      item->submenu()->moving = moving;
      which_sub = index;
    } else
      which_sub = -1;
  }
}


Bool Basemenu::hasSubmenu(int index) {
  if ((index >= 0) && (index < menuitems->count()))
    if (menuitems->find(index)->submenu())
      return True;
    else
      return False;
  else
    return False;
}


void Basemenu::drawItem(int index, Bool highlight, Bool clear,
			 int x, int y, unsigned int w, unsigned int h)
{
  if (index < 0 || index > menuitems->count()) return;

  BasemenuItem *item = menuitems->find(index);
  if (! item) return;
  
  Bool dotext = True, dohilite = True, dosel = True;
  const char *text = (item->ulabel()) ? *item->ulabel() : item->label();
  int sbl = index / menu.persub, i = index - (sbl * menu.persub);
  int item_x = (sbl * menu.item_w), item_y = (i * menu.item_h);
  int hilite_x = item_x, hilite_y = item_y, hoff_x = 0, hoff_y = 0;
  int text_x = 0, text_y = 0, len = strlen(text), sel_x = 0, sel_y = 0;
  unsigned int hilite_w = menu.item_w, hilite_h = menu.item_h, text_w = 0, text_h = 0;
  unsigned int half_w = menu.item_h / 2, quarter_w = menu.item_h / 4;
  
  if (text) {
    text_w = XTextWidth(bbtool->getResource()->getTitleFont(), text, len);
    text_y =  item_y +
		  bbtool->getResource()->getTitleFont()->ascent +
		  (menu.bevel_w / 2);
    
    switch(bbtool->getResource()->getJustification()) {
    case LeftJustify:
      text_x = item_x + menu.bevel_w + menu.item_h + 1;
      break;
      
    case RightJustify:
      text_x = item_x + menu.item_w - (menu.item_h + menu.bevel_w + text_w);
      break;
      
    case CenterJustify:
      text_x = item_x + ((menu.item_w + 1 - text_w) / 2);
      break;
    }
    
    text_h = menu.item_h - menu.bevel_w;
  }
  
  GC gc =
    ((highlight || item->isSelected()) ? bbtool->getMenuHiGC() :
     bbtool->getMenuHiGC()),
    tgc =
    ((highlight) ? bbtool->getMenuHiGC() :
     ((item->isEnabled()) ? bbtool->getMenuFrameGC() :
      bbtool->getMenuFrameGC()));
  
  sel_x = item_x;
  if (bbtool->getResource()->getBulletPosition() == Right)
    sel_x += (menu.item_w - menu.item_h - menu.bevel_w);
  sel_x += quarter_w;
  sel_y = item_y + quarter_w;
  
  if (clear) {
    XClearArea(display, menu.frame, item_x, item_y, menu.item_w, menu.item_h,
	       False);
  } else if (! (x == y && y == -1 && w == h && h == 0)) {
    // calculate the which part of the hilite to redraw
    if (! (max(item_x, x) <= (signed) min(item_x + menu.item_w, x + w) &&
	   max(item_y, y) <= (signed) min(item_y + menu.item_h, y + h))) {
      dohilite = False;
    } else {
      hilite_x = max(item_x, x);
      hilite_y = max(item_y, y);
      hilite_w = min(item_x + menu.item_w, x + w) - hilite_x;
      hilite_h = min(item_y + menu.item_h, y + h) - hilite_y;
      hoff_x = hilite_x % menu.item_w;
      hoff_y = hilite_y % menu.item_h;
    }
    
    // check if we need to redraw the text    
    int text_ry = item_y + (menu.bevel_w / 2);
    if (! (max(text_x, x) <= (signed) min(text_x + text_w, x + w) &&
	   max(text_ry, y) <= (signed) min(text_ry + text_h, y + h)))
      dotext = False;
    
    // check if we need to redraw the select pixmap/menu bullet
    if (! (max(sel_x, x) <= (signed) min(sel_x + half_w, x + w) &&
	   max(sel_y, y) <= (signed) min(sel_y + half_w, y + h)))
      dosel = False;
  }
  
  if (dohilite && highlight && (menu.hilite_pixmap != ParentRelative)) {
    if (menu.hilite_pixmap)
      XCopyArea(display, menu.hilite_pixmap, menu.frame,
		bbtool->getMenuHiBGGC(), hoff_x, hoff_y,
		hilite_w, hilite_h, hilite_x, hilite_y);
    else
      XFillRectangle(display, menu.frame,
		     bbtool->getMenuHiBGGC(),
		     hilite_x, hilite_y, hilite_w, hilite_h);
  } else if (dosel && item->isSelected() &&
	                 (menu.sel_pixmap != ParentRelative)) {
    if (menu.sel_pixmap)
      XCopyArea(display, menu.sel_pixmap, menu.frame,
		bbtool->getMenuHiBGGC(), 0, 0,
		half_w, half_w, sel_x, sel_y);
    else
      XFillRectangle(display, menu.frame,
		     bbtool->getMenuHiBGGC(),
		     sel_x, sel_y, half_w, half_w);
  }
  
  if (dotext && text) {
    XDrawString(display, menu.frame, tgc, text_x, text_y, text, len);
  }

  if (dosel && item->submenu()) {
    switch (bbtool->getResource()->getBulletStyle()) {
    case Square:
      XDrawRectangle(display, menu.frame, gc, sel_x, sel_y, half_w, half_w);
      break;

    case Triangle:
      XPoint tri[3];

      if (bbtool->getResource()->getBulletPosition() == Right) {
        tri[0].x = sel_x + quarter_w - 2;
	tri[0].y = sel_y + quarter_w - 2;
        tri[1].x = 4;
	tri[1].y = 2;
        tri[2].x = -4;
	tri[2].y = 2;
      } else {
        tri[0].x = sel_x + quarter_w - 2;
	tri[0].y = item_y + half_w;
        tri[1].x = 4;
	tri[1].y = 2;
        tri[2].x = 0;
	tri[2].y = -4;
      }
      
      XFillPolygon(display, menu.frame, gc, tri, 3, Convex,
                   CoordModePrevious);
      break;
      
    case Diamond:
      XPoint dia[4];

      dia[0].x = sel_x + quarter_w - 3;
      dia[0].y = item_y + half_w;
      dia[1].x = 3;
      dia[1].y = -3;
      dia[2].x = 3;
      dia[2].y = 3;
      dia[3].x = -3;
      dia[3].y = 3;

      XFillPolygon(display, menu.frame, gc, dia, 4, Convex,
                   CoordModePrevious);
      break;
    }
  }
}


void Basemenu::setLabel(const char *l) {
  if (menu.label)
    delete [] menu.label;

  if (l) menu.label = bstrdup(l);
  else menu.label = 0;
}


void Basemenu::setItemSelected(int index, Bool sel) {
  if (index < 0 || index >= menuitems->count()) return;

  BasemenuItem *item = find(index);
  if (! item) return;

  item->setSelected(sel);
  if (visible) drawItem(index, (index == which_sub), True);
}


Bool Basemenu::isItemSelected(int index) {
  if (index < 0 || index >= menuitems->count()) return False;

  BasemenuItem *item = find(index);
  if (! item) return False;

  return item->isSelected();
}


void Basemenu::setItemEnabled(int index, Bool enable) {
  if (index < 0 || index >= menuitems->count()) return;

  BasemenuItem *item = find(index);
  if (! item) return;

  item->setEnabled(enable);
  if (visible) drawItem(index, (index == which_sub), True);
}


Bool Basemenu::isItemEnabled(int index) {
  if (index < 0 || index >= menuitems->count()) return False;

  BasemenuItem *item = find(index);
  if (! item) return False;

  return item->isEnabled();
}


void Basemenu::buttonPressEvent(XButtonEvent *be) {
  if (be->window == menu.frame) {
    int sbl = (be->x / menu.item_w), i = (be->y / menu.item_h);
    int w = (sbl * menu.persub) + i;

    if (w < menuitems->count() && w >= 0) {
      which_press = i;
      which_sbl = sbl;

      BasemenuItem *item = menuitems->find(w);

      if (item->submenu())
	drawSubmenu(w);
      else
	drawItem(w, (item->isEnabled()), True);
    }
  } else {
    menu.x_move = be->x_root - menu.x;
    menu.y_move = be->y_root - menu.y;
  }
}


void Basemenu::buttonReleaseEvent(XButtonEvent *re) {
  if (re->window == menu.title) {
    if (moving) {
      moving = False;
      
      if (which_sub != -1)
	drawSubmenu(which_sub);
    }
    
    if (re->x >= 0 && re->x <= (signed) menu.width &&
	re->y >= 0 && re->y <= (signed) menu.title_h)
      if (re->button == 3)
	hide();
  } else if (re->window == menu.frame &&
	     re->x >= 0 && re->x < (signed) menu.width &&
	     re->y >= 0 && re->y < (signed) menu.frame_h) {
    if (re->button == 3) {
      hide();
    } else {
      int sbl = (re->x / menu.item_w), i = (re->y / menu.item_h),
	   ix = sbl * menu.item_w, iy = i * menu.item_h,
	    w = (sbl * menu.persub) + i,
	    p = (which_sbl * menu.persub) + which_press;

      if (w < menuitems->count() && w >= 0) {
	drawItem(p, (p == which_sub), True);

        if  (p == w && isItemEnabled(w)) {
	  if (re->x > ix && re->x < (signed) (ix + menu.item_w) &&
	      re->y > iy && re->y < (signed) (iy + menu.item_h)) {
	    itemSelected(re->button, w);
	  }
        }
      } else
        drawItem(p, False, True);
    }
  }
}


void Basemenu::motionNotifyEvent(XMotionEvent *me) {
  if (me->window == menu.title && (me->state & Button1Mask)) {
    if (movable) {
      if (! moving) {
	if (parent && (! torn)) {
	  parent->drawItem(parent->which_sub, False, True);
	  parent->which_sub = -1;
	}

        moving = torn = True;

	if (which_sub != -1)
	  drawSubmenu(which_sub);
      } else {
	menu.x = me->x_root - menu.x_move,
	menu.y = me->y_root - menu.y_move;
	
	XMoveWindow(display, menu.window, menu.x, menu.y);
	  
	if (which_sub != -1)
	  drawSubmenu(which_sub);
      }
    }
  } else if ((! (me->state & Button1Mask)) && me->window == menu.frame &&
	     me->x >= 0 && me->x < (signed) menu.width &&
	     me->y >= 0 && me->y < (signed) menu.frame_h) {
    int sbl = (me->x / menu.item_w), i = (me->y / menu.item_h),
	  w = (sbl * menu.persub) + i;

    if ((i != which_press || sbl != which_sbl) &&
	(w < menuitems->count() && w >= 0)) {
      if (which_press != -1 && which_sbl != -1) {
	int p = (which_sbl * menu.persub) + which_press;
	BasemenuItem *item = menuitems->find(p);
	drawItem(p, False, True);
	if (item->submenu())
	  if (item->submenu()->isVisible() &&
	      (! item->submenu()->isTorn())) {
	    item->submenu()->internal_hide();
	    which_sub = -1;
	  }
      }

      which_press = i;
      which_sbl = sbl;

      BasemenuItem *itmp = menuitems->find(w);

      if (itmp->submenu())
	drawSubmenu(w);
      else {
	drawItem(w, (itmp->isEnabled()), True);
	setHighlight(w);
			}
    }
  }
}


void Basemenu::exposeEvent(XExposeEvent *ee) {
  if (ee->window == menu.title) {
    redrawTitle();
  } else if (ee->window == menu.frame) {
    LinkedListIterator<BasemenuItem> it(menuitems);

    // this is a compilicated algorithm... lets do it step by step...
    // first... we see in which sub level the expose starts... and how many
    // items down in that sublevel

    int sbl = (ee->x / menu.item_w), id = (ee->y / menu.item_h),
      // next... figure out how many sublevels over the redraw spans
      sbl_d = ((ee->x + ee->width) / menu.item_w),
      // then we see how many items down to redraw
      id_d = ((ee->y + ee->height) / menu.item_h);

    if (id_d > menu.persub) id_d = menu.persub;

    // draw the sublevels and the number of items the exposure spans
    int i, ii;
    for (i = sbl; i <= sbl_d; i++) {
      // set the iterator to the first item in the sublevel needing redrawing
      it.set(id + (i * menu.persub));
      for (ii = id; ii <= id_d && it.current(); it++, ii++) {
	int index = ii + (i * menu.persub);
	// redraw the item
	drawItem(index, (which_sub == index), False,
		 ee->x, ee->y, ee->width, ee->height);
      }
    }
  }
}


void Basemenu::enterNotifyEvent(XCrossingEvent *ce) {
  if (ce->window == menu.frame) {
    menu.x_shift = menu.x, menu.y_shift = menu.y;
    if (menu.x + menu.width > bbtool->getCurrentScreenInfo()->getWidth()) {
      menu.x_shift = bbtool->getCurrentScreenInfo()->getWidth() - menu.width -
        bbtool->getResource()->getBorderWidth();
      shifted = True;
    } else if (menu.x < 0) {
      menu.x_shift = -bbtool->getResource()->getBorderWidth();
      shifted = True;
    }

    if (menu.y + menu.height > bbtool->getCurrentScreenInfo()->getHeight()) {
      menu.y_shift = bbtool->getCurrentScreenInfo()->getHeight() - menu.height -
        bbtool->getResource()->getBorderWidth();
      shifted = True;
    } else if (menu.y + (signed) menu.title_h < 0) {
      menu.y_shift = -bbtool->getResource()->getBorderWidth();
      shifted = True;
    }

    if (shifted)
      XMoveWindow(display, menu.window, menu.x_shift, menu.y_shift);

    if (which_sub != -1) {
      BasemenuItem *tmp = menuitems->find(which_sub);
      if (tmp->submenu()->isVisible()) {
	int sbl = (ce->x / menu.item_w), i = (ce->y / menu.item_h),
	  w = (sbl * menu.persub) + i;

	if (w != which_sub && (! tmp->submenu()->isTorn())) {
	  tmp->submenu()->internal_hide();

	  drawItem(which_sub, False, True);
	  which_sub = -1;
	}
      }
    }
  }
}


void Basemenu::leaveNotifyEvent(XCrossingEvent *ce) {
  if (ce->window == menu.frame) {
    if (which_press != -1 && which_sbl != -1 && menuitems->count() > 0) {
      int p = (which_sbl * menu.persub) + which_press;

      drawItem(p, (p == which_sub), True);

      which_sbl = which_press = -1;
    }

    if (shifted) {
      XMoveWindow(display, menu.window, menu.x, menu.y);
      shifted = False;

      if (which_sub != -1) drawSubmenu(which_sub);
    }
  }
}


void Basemenu::reconfigure(void) {
  XSetWindowBackground(display, menu.window,
		       bbtool->getResource()->getBorderColor()->getPixel());
  XSetWindowBorder(display, menu.window,
		   bbtool->getResource()->getBorderColor()->getPixel());
  XSetWindowBorderWidth(display, menu.window, bbtool->getResource()->getBorderWidth());

  menu.bevel_w = bbtool->getResource()->getBevelWidth();
  update();
}