// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- ScreenHandler.h --
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

#ifndef  SCREENHANDLER_HH
#define  SCREENHANDLER_HH

extern "C" {
  #include <X11/Xlib.h>
}

#include <list>
#include <iostream>
#include <string>

#include "KeyClient.h"
#include "WindowlistMenu.h"
#include "KeyGrabber.h"
#include "Config.h"
#include "keytree.hh"
#include "Netclient.h"
#include "window.hh"

// the blackbox library
#include <Display.hh>
#include <EventHandler.hh>
#include <Netwm.hh>

class KeyClient;
class WindowlistMenu;
class keytree;

class ScreenHandler : public bt::EventHandler
{

public:
  ScreenHandler (KeyClient * _k, unsigned int number);
  ~ScreenHandler () ;
  inline unsigned int getScreenNumber() const { return _screenNumber; }
  inline unsigned int getDesktopNumber() const { return _active_desktop; }
  std::string getDesktopName(unsigned int desktopNbr) const;
  inline Window getRootWindow() const { return _screenInfo.rootWindow(); }
  inline KeyClient & getKeyClient() const { return * _keyClient; }
  inline bool isManaged() const { return _managed; }
  inline const bt::ScreenInfo & getScreenInfo() { return _screenInfo; }

  void initialize();
  void grabKey(const KeyCode keyCode, const int modifierMask) const;
  void ungrabKey(const KeyCode keyCode, const int modifierMask) const;

  const XWindow *lastActiveWindow() const;
  
private:
  friend class WindowlistMenu;
  bool _managed;
  bool _debug;
  bool _grabbed; // used for keygrab toggle function
  bool _cycling; // used for window cycling
  bool _raise_while_cycling;
  bool _show_cycle_menu;
  bool _honor_modifiers;
  std::string _menu_text_justify;
  unsigned int _workspace_columns;
  unsigned int _workspace_rows;
  
  unsigned int _screenNumber;
  KeyClient * _keyClient;
  bt::Netwm * _netwm;
  Netclient * _netclient;
  Config * _config;
  Display * _display;
  Window _root;
  KeyGrabber * _keyGrabber;
  keytree * _keybindings;
  int _numlockMask;
  int _scrolllockMask;
  std::string _wm_name;
  WindowlistMenu * _windowmenu;
  

  unsigned int _num_desktops;
  unsigned int _active_desktop;
  bt::Netwm::UTF8StringList _desktop_names;

  
  const bt::ScreenInfo & _screenInfo;

  bool careAboutWindow(Window window) const ;
  XWindow * findWindow(Window window) const ;
  void p(void);

  void keyPressEvent (const XKeyEvent * const e) ;
  void keyReleaseEvent (const XKeyEvent * const e) ;
  void propertyNotifyEvent(const XPropertyEvent * const e);
  void updateNumDesktops();
  void updateDesktopNames();
  void updateActiveDesktop();
  void updateActiveWindow();
  void updateClientList();
  void updateClientListStacking();

  WindowList & _clients;
  WindowList::iterator & _active;
  WindowList::iterator _last_active;

  bool nothingIsPressed(void) const;
  bool findSupportingWM();

  void cycleWindow(unsigned int state, const bool forward, const int increment,
                   const bool allscreens = false,
                   const bool alldesktops = false,
                   const bool sameclass = false,
                   const std::string &classname = "");
  WindowList getCycleWindowList(unsigned int state, const bool forward, const int increment,
                   const bool allscreens = false,
                   const bool alldesktops = false,
                   const bool sameclass = false,
                   const std::string &classname = "");  
  void cycleWorkspace(const bool forward, const int increment,
                      const bool loop = true) const;
  void changeWorkspace(const int num) const;
  void changeWorkspaceVert(const int num) const;
  void changeWorkspaceHorz(const int num) const;

  void execCommand(const std::string &cmd) const;

};
#endif
