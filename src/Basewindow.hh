//  Basewindow.hh for bbtools
//
//  Copyright (c) 1998-1999 by John Kennis, jkennis@chello.nl
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// (See the included file COPYING / GPL-2.0)
//
// $Id: Basewindow.hh,v 1.5 2002/10/08 02:06:41 vanrijn Exp $

#ifndef __BASEWINDOW_HH
#define __BASEWINDOW_HH

#ifdef HAVE_STDIO_H
#  include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#  include <string.h>
#endif

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#ifdef HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif

#ifdef HAVE_MATH_H
#  include <math.h>
#endif

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
}

#include "BaseDisplay.hh"
#include "main.hh"
#include "Image.hh"

class Resource;
class BaseDisplay;

class Basewindow : public BaseDisplay {

public:
  Basewindow(int, char**,struct CMDOPTIONS *);
  virtual ~Basewindow();



  void setupImageControl(void);

  enum { B_LeftJustify = 1, B_RightJustify, B_CenterJustify };

  BImageControl *getImageControl(void) { return image_control; }

  BColor border_color;

  int colors_per_channel;
  bool image_dither;

  Atom wm_delete_window;
  char *position;
  char *config_filename;
  char *config_file;
  char *style_file;
  char *spooldir;
  bool nobb_config;
  bool withdrawn;
  bool honor_modifiers;
  bool iconic;
  bool shape;
  bool decorated;
  bool noQt;
  bool miniMe;
  bool tinyMe;
  ScreenInfo *getCurrentScreenInfo(void) { return current_screen_info; }
  int getArgc(void) { return iargc; }
  char ** getArgv(void) { return iargv; }

protected:
  virtual void process_event(XEvent *);
  virtual void reconfigure(void) = 0;
  Bool handleSignal(int);


  
  GC menuGC;
  GC menuHiBGGC;
  GC menuHiGC;
  GC menuFrameGC;
  Window menuwin;
  ScreenInfo *current_screen_info;
 
private: 
  BImageControl *image_control;
  int iargc;
  char **iargv;
};

#endif /* __BASEWINDOW_HH */
