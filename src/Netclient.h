// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- Netclient.h --
// Copyright (c) 2001 - 2003 Jason 'vanRijn' Kasper <vR at movingparts dot net>
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

// E_O_H_VR

#ifndef  NETCLIENT_HH
#define  NETCLIENT_HH

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xatom.h>
}

#include <assert.h>

#include <vector>
#include <string>
#include <algorithm>

// blackbox lib
#include <Netwm.hh>

class Netclient : public bt::Netwm
{

private:
  Display * _display;

  Atom xa_wm_colormap_windows, xa_wm_protocols, xa_wm_state,
    xa_wm_delete_window, xa_wm_take_focus, xa_wm_change_state,
    motif_wm_hints, xa_wm_name, xa_wm_class;

  Atom openbox_show_root_menu, openbox_show_workspace_menu;

  Atom blackbox_attributes, blackbox_change_attributes, blackbox_hints;

  bool getValue(Window win, Atom atom, Atom type,
                unsigned long &nelements, unsigned char **value,
                int size) const;

  void init_icccm(void);
  void init_extras(void);
  void init_blackbox(void);

public:

  Netclient (Display * display);
  ~Netclient () ;

  // various value accessors...
  std::string getWindowTitle(Window win) const;
  unsigned int getDesktop(Window win) const;


  // atoms Netwm doesn't provide

  // icccm first
  inline Atom xaWmColormapWindows(void) const {return xa_wm_colormap_windows;}
  inline Atom xaWmProtocols(void) const {return xa_wm_protocols;}
  inline Atom xaWmState(void) const {return xa_wm_state;}
  inline Atom xaWmClass(void) const {return xa_wm_class;}
  inline Atom xaWmName(void) const {return xa_wm_name;}
  inline Atom xaWmDeleteWindow(void) const {return xa_wm_delete_window;}
  inline Atom xaWmTakeFocus(void) const {return xa_wm_take_focus;}
  inline Atom xaWmChangeState(void) const {return xa_wm_change_state;}
  inline Atom xaMotifWmHints(void) const {return motif_wm_hints;}

  // extra goodies next
  inline Atom xaOpenboxShowRootMenu(void) const {return openbox_show_root_menu;}
  inline Atom xaOpenboxShowWorkspaceMenu(void) const
    {return openbox_show_workspace_menu;}

  // old blackbox atoms
  inline Atom xaBlackboxAttributes(void) const {return blackbox_attributes;}
  inline Atom xaBlackboxChangeAttributes(void) const {return blackbox_change_attributes;}
  inline Atom xaBlackboxHints(void) const {return blackbox_hints;}
                                                                    
  
  enum StringType {
    ansi,
    utf8,
    NUM_STRING_TYPE
  };

  typedef std::vector<std::string> StringVect;

  bool getValue(Window win, Atom atom, Atom type,
                unsigned long &nelements, unsigned long **value) const;
  bool getValue(Window win, Atom atom, Atom type, unsigned long &value) const;
  bool getValue(Window win, Atom atom, StringType type,
                std::string &value) const;
  bool getValue(Window win, Atom atom, StringType type,
                unsigned long &nelements, StringVect &strings) const;
  
  void eraseValue(Window win, Atom atom) const;

  // sends a client message a window
  void sendClientMessage(Window target, Atom type, Window about,
                         long data = 0, long data1 = 0, long data2 = 0,
                         long data3 = 0, long data4 = 0) const;
  

};
#endif
