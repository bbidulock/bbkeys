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
  _config = s->getKeyClient().getConfig();
  _debug = _config->getBoolValue("debug", false);
  _screen_info = & s->getScreenInfo();
  _honor_modifiers = _config->getBoolValue("honormodifiers", false);
  _screen->getKeyClient().getLockModifiers(numLockMask, scrollLockMask);
}

void WindowlistMenu::keyPressEvent (const XKeyEvent * const e) {

  unsigned int state = e->state;

  if (_debug)
    std::cout << "WindowlistMenu: got keyPressEvent!" << std::endl;

  if (!_honor_modifiers) {
    state = e->state & ~(LockMask|scrollLockMask|numLockMask);
  }

  const Action *it = _keybindings->getAction(e, state, _screen);

  if (it) {
    switch (it->type()) {

    case Action::nextWindow:
    case Action::nextWindowOnAllWorkspaces:
    case Action::nextWindowOnAllScreens:
    case Action::nextWindowOfClass:
    case Action::nextWindowOfClassOnAllWorkspaces:
      selectNext();
      break;

    case Action::prevWindow:
    case Action::prevWindowOnAllWorkspaces:
    case Action::prevWindowOnAllScreens:
    case Action::prevWindowOfClass:
    case Action::prevWindowOfClassOnAllWorkspaces:
      selectPrevious();
      break;

    default:
      break;
    }
  }

  // if the user is cancelling the menu/cycle, then set focus back on the
  // window they started with.
  if (e->keycode == XKeysymToKeycode(_display, XK_Escape)) {
    XWindow * win = dynamic_cast<XWindow *>(*_windowList.begin());
    win->focus(true);
  } else if (e->keycode == XKeysymToKeycode(_display, XK_Up)) {
    selectPrevious(false);
  } else if (e->keycode == XKeysymToKeycode(_display, XK_Down)) {
    selectNext(false);
  }

  bt::Menu::keyPressEvent(e);

}

void WindowlistMenu::keyReleaseEvent (const XKeyEvent * const e) {

  if (_debug)
    std::cout << "WindowlistMenu: got keyReleaseEvent!" << std::endl;

  if (_screen->nothingIsPressed() ){
    // get what window is selected so we can focus it
    XWindow * win = getSelectedWindow();

    bt::Menu::hide();
    _screen->keyReleaseEvent(e);

    // if the user is ending his window-cycling adventure, then raise the window
    // for him.
    if (win)
      win->focus(true);
  }

  bt::Menu::keyReleaseEvent(e);

}

void WindowlistMenu::showCycleMenu( WindowList theList ) {

  bt::Menu::clear();
  bt::Menu::setTitle("Switch To...");
  bt::Menu::showTitle();

  _windowList = theList;

  WindowList::const_iterator it;
  const WindowList::const_iterator end = theList.end();

  // first, find out if we have any windows in our list for anything other
  // than the current desktop
  _desktop_nbr = _screen->getDesktopNumber();
  bool onlyThisDesktop = true;
  for (it = theList.begin(); it != end; it++) {
    unsigned int dNbr = (*it)->desktop();
    if ( (dNbr != _desktop_nbr) && (0xFFFFFFFF != dNbr) ) {
      onlyThisDesktop = false;
      break;
    }
  }

  // now add the windows to our list
  unsigned int i = 0;

  for (it = theList.begin(); it != end; it++) {
    std::string title = (*it)->title();
    unsigned int dNbr = (*it)->desktop();
    std::string newTitle = bt::ellideText(title, 100, " ... ");
    if (! onlyThisDesktop) {
      std::string suffix = _screen->getDesktopName(dNbr);
      newTitle.append(" (" + suffix + ")");
    }
    bt::Menu::insertItem( newTitle, i++ );
  }

  // this is our current window, before cycling.  set it checked as a
  // visual indicator
  bt::Menu::setItemChecked(0, true);

  int x = _config->getNumberValue("cyclemenux", _screen_info->width() /2);
  int y = _config->getNumberValue("cyclemenuy", _screen_info->height() /2);

  // now show the menu
  bt::Menu::popup(x, y, false);
  bt::Menu::move(x,y);

  // reset our marker as we will increment it in selectNext...
  _current_index = -1;

  // we don't have anything selected initially, so we need to set the
  // selection to the second one (the first one is the
  // currently-selected window
  selectNext();
  selectNext();

}

void WindowlistMenu::itemClicked(unsigned int id, unsigned int button) {

  WindowList::const_iterator it = _windowList.begin();
  const WindowList::const_iterator end = _windowList.end();

  unsigned int x = 0;
  for (; it != end; ++it) {
    XWindow * const win = dynamic_cast<XWindow *>(*it);
    if ( id == x++ ) {
      win->focus(true);
    }
  }

}

void WindowlistMenu::selectNext(bool manual) {

  // keep track of where we are...
  trackIndex(1);

  XWindow * win = getSelectedWindow();
  if (win) win->focus(false);

  if (manual) {
    XKeyEvent neo;
    KeyCode keyCode = XKeysymToKeycode(_display, XK_Down);
    neo.keycode = keyCode;
    bt::Menu::keyPressEvent(&neo);
  }

}

void WindowlistMenu::selectPrevious(bool manual) {

  // keep track of where we are now...
  trackIndex(-1);

  XWindow * win = getSelectedWindow();
  if (win) win->focus(false);

  if (manual) {
    XKeyEvent neo;
    KeyCode keyCode = XKeysymToKeycode(_display, XK_Up);
    neo.keycode = keyCode;
    bt::Menu::keyPressEvent(&neo);
  }

}

void WindowlistMenu::trackIndex (const int movement) {

  if (movement > 0) {
    if (static_cast<unsigned int>(++_current_index) >= _windowList.size() )
      _current_index = 0;
  } else {
    if (--_current_index < 0)
      _current_index = _windowList.size() -1;
  }

}

XWindow * WindowlistMenu::getSelectedWindow() {

  WindowList::const_iterator it = _windowList.begin();
  const WindowList::const_iterator end = _windowList.end();

  XWindow * win = 0;

  unsigned int x = 0;
  for (; it != end; it++) {
    if ( static_cast<unsigned int>(_current_index) == x++ ) {
      win = dynamic_cast<XWindow *>(*it);
    }
  }

  if (0 == win)
    std::cerr << "WindowlistMenu: getSelectedWindow--couldn't get window.  this won't turn out well.\n";

  if (_debug && win)
    std::cout << "WindowlistMenu: getSelectedWindow: currently-selected window: ["
              << win->title() << "]\n";
  return win;

}
