//  NETInterface.cc for bbtools.
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
// $Id: NETInterface.cc,v 1.4 2002/05/30 20:47:45 eckzor Exp $

#ifdef    HAVE_CONFIG_H
#  include "../config.h"
#endif // HAVE_CONFIG_H

#include "NETInterface.hh"
#include "Basewindow.hh"

NETInterface::NETInterface(Basewindow *basewindow) {

  timer=new BTimer(basewindow->getCurrentScreenInfo()->getBaseDisplay(),this) ;
  base=basewindow;
  net_init=False;
  timer->setTimeout(10000);
  timer->fireOnce(True);
  timer->start();
}

void NETInterface::handleNETEvents(XEvent Event) {
  if (Event.xclient.message_type==base->getBlackboxStructureMessagesAtom()) {
    if ((unsigned)Event.xclient.data.l[0]==base->getBlackboxNotifyStartupAtom()) {
      NETNotifyStartup();
      net_init=True;
    }
    else if ((unsigned)Event.xclient.data.l[0]==
                                      base->getBlackboxNotifyWindowAddAtom()) {
      NETNotifyWindowAdd(Event.xclient.data.l[1],Event.xclient.data.l[2]);
    }
    else if ((unsigned)Event.xclient.data.l[0]==
                                      base->getBlackboxNotifyWindowDelAtom()) {
      NETNotifyDel(Event.xclient.data.l[1]);
    }
    else if ((unsigned)Event.xclient.data.l[0]==
                                      base->getBlackboxAttributesAtom()) {
      NETNotifyAttributes(Event.xclient.data.l[1]);
    }
    else if ((unsigned)Event.xclient.data.l[0]==
                                      base->getBlackboxNotifyWindowFocusAtom()){
      NETNotifyFocus(Event.xclient.data.l[1]);
    }
    if ((unsigned)Event.xclient.data.l[0]==
                                   base->getBlackboxNotifyCurrentWorkspaceAtom()){
      NETNotifyCurrentWorkspace(Event.xclient.data.l[1]);
    }
    else if ((unsigned)Event.xclient.data.l[0]==
                                    base->getBlackboxNotifyWorkspaceCountAtom()) {
      NETNotifyWorkspaceCount(Event.xclient.data.l[1]);
    }
  }
}

void NETInterface::timeout() {
  if (!net_init) {
    fprintf(stderr,"Cannot connect to window manager\n");
/*  we'll comment it out for now... I'm not so sure I want to just die out...
    base->shutdown();
*/
  }
}
