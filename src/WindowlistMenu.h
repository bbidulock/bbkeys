// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- WindowlistMenu.h --
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

#ifndef  WINDOWLISTMENU_HH
#define  WINDOWLISTMENU_HH

extern "C" {
#include <X11/Xlib.h>
}

#include <Menu.hh>

#include "ScreenHandler.h"
#include "window.hh"
#include "keytree.hh"

class ScreenHandler;
class keytree;

class WindowlistMenu : public bt::Menu
{

public:
  WindowlistMenu (ScreenHandler * s);
  ~WindowlistMenu (void) ;
  void showCycleMenu (WindowList theList ) ;
  void itemClicked(unsigned int id, unsigned int button);

private:
  ScreenHandler * _screen;
  WindowList  _windowList;
  keytree * _keybindings;
  Display * _display;
  void keyPressEvent (const XKeyEvent * const e) ;
  void keyReleaseEvent (const XKeyEvent * const e) ;

};
#endif
