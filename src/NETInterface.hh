//  NETInterface.hh for bbtools.
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
// $Id: NETInterface.hh,v 1.4 2002/10/08 02:06:41 vanrijn Exp $

#ifndef __NETINTERFACE_HH
#define __NETINTERFACE_HH

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
}

#include "Timer.hh"

class Basewindow;

class NETInterface : public TimeoutHandler {
  public:
    NETInterface(Basewindow *);
    virtual ~NETInterface(void) {};
    void handleNETEvents(XEvent);

  protected:
    virtual void NETNotifyStartup(void) {net_init=True;}
    virtual void NETNotifyWindowAdd(Window,int)=0;
    virtual void NETNotifyDel(Window)=0;
    virtual void NETNotifyAttributes(Window)=0;
    virtual void NETNotifyFocus(Window)=0;
    virtual void NETNotifyCurrentWorkspace(int)=0;
    virtual void NETNotifyWorkspaceCount(int)=0;
    virtual void timeout(void);

  private:
    Basewindow *base;
    bool net_init;
    BTimer *timer;
};


#endif // __NETINTERFACE_HH
