// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- WindowlistMenu.cpp --
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

#include "WindowlistMenu.h"
//--------------------------------------------------------
// Constructor/Destructor 
//--------------------------------------------------------
WindowlistMenu::WindowlistMenu (ScreenHandler * s) : 
  bt::Menu( s->getKeyClient().getMainApplication(),
    s->getScreenNumber() ) {
  _screen = s;
  _keybindings = s->getKeyClient().getKeybindings();
  _display = s->getKeyClient().XDisplay();
}
WindowlistMenu::~WindowlistMenu () {
}

void WindowlistMenu::keyPressEvent (const XKeyEvent * const e) {

  unsigned int state = e->state;
  
  // if we've made it this far, handle the action....
  const Action *it = _keybindings->getAction(e, state, _screen);

  XKeyEvent * neo = new XKeyEvent;

  KeyCode keyCode;
  
  if (it) {
    switch (it->type()) {

    case Action::nextWindow:
      keyCode = XKeysymToKeycode(_display, XK_Down);
      neo->keycode = keyCode;
      _screen->cycleWindow(state, true, it->number() != 0 ? it->number(): 1);
      break;
      
    case Action::prevWindow:
      keyCode = XKeysymToKeycode(_display, XK_Up);
      neo->keycode = keyCode;
      _screen->cycleWindow(state, false, it->number() != 0 ? it->number(): 1);
      break;
    }    
  }
  bt::Menu::keyPressEvent(neo);

}

void WindowlistMenu::keyReleaseEvent (const XKeyEvent * const e) {

  bt::Menu::keyReleaseEvent(e);

  if (_screen->nothingIsPressed() ){
    bt::Menu::hide();
    _screen->keyReleaseEvent(e);
  }
  
}

void WindowlistMenu::showCycleMenu( WindowList theList ) {
  
  bt::Menu::clear();
  bt::Menu::setTitle("Switch To...");
  bt::Menu::showTitle();

  _windowList = theList;

  WindowList::const_iterator it = theList.begin();
  const WindowList::const_iterator end = theList.end();
  
  for (; it != end; ++it) {
    bt::Menu::insertItem( (*it)->title(), (*it)->window() );
  }

  bt::Menu::popup(50, 50);

}

void WindowlistMenu::itemClicked(unsigned int id, unsigned int button) {
  
  WindowList::const_iterator it = _windowList.begin();
  const WindowList::const_iterator end = _windowList.end();

  for (; it != end; ++it) {
    XWindow * const win = dynamic_cast<XWindow *>(*it);
    if ( id == win->window() ) {
      std::cout << "yay! you selected id: [" << id
		<<"], window: [" << win->title() << "]" << std::endl;
      win->focus(true);
    }
  }
  
}
