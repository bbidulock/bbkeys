// Baseresource.hh for bbtools - tools to display resources in X11.
//
//  Copyright (c) 1998-1999 John Kennis, jkennis@chello.nl
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
// $Id: Baseresource.hh,v 1.5 2002/05/31 17:07:04 eckzor Exp $

#ifndef __BASERESOURCE_HH
#define __BASERESOURCE_HH

#ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include <X11/Xlib.h>
#include <X11/Xresource.h>

#include "Timer.hh"

#define BBTOOLS 1
#define BLACKBOX 2

#define BBTOOL_LOCAL ".bbtools/bbkeys.nobb"
#define BLACKBOX_LOCAL ".bbtools/bbkeys.bb"

class ToolWindow;
class BImageControl;
class BColor;
class BTexture;

struct STYLE {
  bool auto_config;
  bool honor_modifiers;
  char *conf_filename;
  char *style_filename;
  time_t  mtime;
};

class BaseResource : public TimeoutHandler {

  public:
    BaseResource(ToolWindow *);
    virtual ~BaseResource(void);

    void CopyColor(BColor *,BColor *);
    void CopyTexture(BTexture ,BTexture *);
    void Reload(void);
    STYLE style;

  protected:
    void Load(void);
    void readDatabaseColor(const char *, const char *, BColor *);
    void readDatabaseTexture(const char *, const char *, BTexture *);
    void readColor(const char *, const char *, const char *, const char *,
									 const char *,BColor *);
    void readTexture(const char *, const char *, const char *, const char *,
										 const char *, const char *, const char *, BTexture *);

  
    ToolWindow *bbtool;
    XrmDatabase resource_db;
    XrmDatabase db;

    virtual void LoadBBToolResource(void) = 0;
    virtual void timeout(void);
  private:
    bool ReadResourceFromFilename(const char *, const char *);
    void ReadBBtoolResource(void);
    void ReadDefaultResource(void);
    void ReadBlackboxResource(void);
    BImageControl *image_control;
    int ResourceType;
    BTimer *timer;
    int check_timeout;
    time_t mtime;
    struct stat file_status;
    bool auto_config;
};

#endif /* __BASERESOURCE_HH */
