// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- ScreenHandler.cpp --
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

#include "../config.h"

extern "C" {
#ifdef    HAVE_STDIO_H
#  include <stdio.h>
#endif // HAVE_STDIO_H

#ifdef    HAVE_UNISTD_H
#  include <sys/types.h>
#  include <unistd.h>
#endif // HAVE_UNISTD_H

#include <X11/keysym.h>
#include <stdlib.h>
}

#include "ScreenHandler.h"

using std::cout;

//--------------------------------------------------------
// Constructor/Destructor
//--------------------------------------------------------
ScreenHandler::ScreenHandler (KeyClient * k, unsigned int number)
  : _managed(true), _screenNumber(number),
    _screenInfo(k->display().screenInfo(number)),
    _clients(k->clientsList()), _active(k->activeWindow())
{
  _keyClient = k;
  _netclient = k->getNetclient();
  _config = k->getConfig();
  _display = k->XDisplay();
  _last_active = _clients.end();
  _keyGrabber = k->getKeyGrabber();
  _keybindings = k->getKeybindings();
  _root = _screenInfo.rootWindow();

  // get our lockmasks from bt::Application
  k->getLockModifiers(_numlockMask, _scrolllockMask);

  // find a window manager supporting NETWM, waiting for it to load if we must
  int count = 20;  // try 20 times
  _managed = false;
  while (! (_keyClient->shuttingDown() || _managed || count <= 0)) {
    if (! (_managed = findSupportingWM()))
      sleep(5);
    --count;
  }

  if (!_managed) {
    cout << BBTOOL << ": " << "ScreenHandler: Unable to find a "
         << "compatible window manager for screen: [" << number << "].\n";
    return;
  }

  bt::Netwm::AtomList atoms;
  if (_netclient->readSupported(_root, atoms)) {
    cout << BBTOOL << ": " << "ScreenHandler: Supported atoms: [" << atoms.size() << "].\n";
  } else {
    cout << BBTOOL << ": " << "ScreenHandler: No supported ewmh hints. Not able to be managed.\n";
    _managed = false;
    return;
  }

  XSelectInput(_display, _root,
               PropertyChangeMask | KeyPressMask | KeyReleaseMask);

  // add an event handler for our root window
  k->insertEventHandler(_root, this);

  // get configuration options
  _honor_modifiers   = _config->getBoolValue("honormodifiers", false);
  _raise_while_cycling = _config->getBoolValue("raisewhilecycling", true);
  _show_cycle_menu   = _config->getBoolValue("showcyclemenu", true);
  _menu_text_justify = _config->getStringValue("menutextjustify", "left");
  _workspace_columns = _config->getNumberValue("workspacecolumns", 0);
  _workspace_rows    = _config->getNumberValue("workspacerows", 0);
  _debug             = _config->getBoolValue("debug", false);

  _cycling = false;

  // our popup window list menu
  _windowmenu = new WindowlistMenu(this);

}
ScreenHandler::~ScreenHandler ()
{
  _keyClient->removeEventHandler( _root );

  _keyGrabber->ungrabAll(_root);

  if (_managed)
    XSelectInput(_display, _root, None);
}

void ScreenHandler::initialize()
{
  _keybindings->grabDefaults(this);

  updateActiveDesktop();
  updateNumDesktops();
  updateDesktopNames();
  updateClientList();
  updateActiveWindow();

  // load graphics resource from config file
  bt::Resource res(_config->getStringValue("stylefile", DEFAULTSTYLE));
  bt::MenuStyle::get(_keyClient->getMainApplication(),
		     _screenNumber)->load(res);


}

bool ScreenHandler::findSupportingWM() {

  if (_debug)
    cout << endl << BBTOOL << ": " << "ScreenHandler: in findSupportingWM."<< endl;

  Window client, tmp;
  bool res = false;

  res = _netclient->readSupportingWMCheck(_root, &client);
  if (!res) {
    if (_debug)
      cout << BBTOOL << ": " << "ScreenHandler: first readSupportingWMCheck failed." << endl;
    return false;
  }

  if (_debug)
    cout << BBTOOL << ": " << "ScreenHandler: first readSupportingWMCheck succeeded." << endl;

  res = _netclient->readSupportingWMCheck(client, &tmp);
  if (!res || client != tmp) {
    if (_debug)
      cout << BBTOOL << ": " << "ScreenHandler: second readSupportingWMCheck failed." << endl;
    return false;
  }

  if (_debug)
    cout << BBTOOL << ": " << "ScreenHandler: second readSupportingWMCheck worked." << endl;

  // now try to get the name of the window manager, using utf8 first
  // and falling back to ansi if that fails


  // try netwm
  if (! _netclient->getValue(client, _netclient->wmName(),
                             Netclient::utf8, _wm_name)) {
    if (_debug)
      cout << BBTOOL << ": " << "ScreenHandler: first try at getting wmName failed." << endl;
    // try old x stuff
    _netclient->getValue(client, XA_WM_NAME, Netclient::ansi, _wm_name);
  }

  if (_wm_name.empty()) {
    if (_debug)
      cout << BBTOOL << ": " << "ScreenHandler: couldn't get wm's name.  letting it slide this time...." << endl;
    _wm_name = "beats the heck out of me";
  }

  cout << BBTOOL << ": " << "ScreenHandler: Found compatible "
       << "window manager: [" << _wm_name << "] for screen: ["
       << _screenNumber << "].\n";

  return true;

}

void ScreenHandler::grabKey(const KeyCode keyCode,
                            const int modifierMask) const {
  _keyGrabber->grab(keyCode, modifierMask, _root );
}

void ScreenHandler::ungrabKey(const KeyCode keyCode,
                              const int modifierMask) const {
  _keyGrabber->ungrab(keyCode, modifierMask, _root );
}

void ScreenHandler::keyPressEvent (const XKeyEvent * const e)
{
  unsigned int state = e->state;

  // Mask out the lock modifiers unless our user doesn't want this
  if (! _honor_modifiers) {
    state= e->state & ~(LockMask|_scrolllockMask|_numlockMask);
  }

  // first, check to see if we're in the middle of a window cycling
  // loop-de-loop and we're getting a cancel....
  if (_cycling && e->keycode == XKeysymToKeycode(_display, XK_Escape)) {

    // we've been told to cancel out of a cycleWindow loop, so we turn
    // off cycling, ungrab the keyboard, then raise the last-active
    // window for our user
    _cycling = false;
    XUngrabKeyboard(_display, CurrentTime);

    const XWindow * la = lastActiveWindow();

    if (la) la->focus(true);

    return;
  }

  // if we've made it this far, handle the action....
  const Action *it = _keybindings->getAction(e, state, this);

  if (!it)
    return;

  switch (it->type()) {

  case Action::chain:
    // if we're doing a chain, then keytree has done everything for us...
    // just return
    return;

  case Action::nextScreen:
    _keyClient->cycleScreen(_screenNumber, true);
    return;

  case Action::prevScreen:
    _keyClient->cycleScreen(_screenNumber, false);
    return;

  case Action::nextWorkspace:
    cycleWorkspace(true, it->number() != 0 ? it->number(): 1);
    return;

  case Action::prevWorkspace:
    cycleWorkspace(false, it->number() != 0 ? it->number(): 1);
    return;

  case Action::nextWindow:
    cycleWindow(state, true, it->number() != 0 ? it->number(): 1);
    return;

  case Action::prevWindow:
    cycleWindow(state, false, it->number() != 0 ? it->number(): 1);
    return;

  case Action::nextWindowOnAllWorkspaces:
    cycleWindow(state, true, it->number() != 0 ? it->number(): 1,  false, true);
    return;

  case Action::prevWindowOnAllWorkspaces:
    cycleWindow(state, false, it->number() != 0 ? it->number(): 1, false, true);
    return;

  case Action::nextWindowOnAllScreens:
    cycleWindow(state, true, it->number() != 0 ? it->number(): 1, true);
    return;

  case Action::prevWindowOnAllScreens:
    cycleWindow(state, false, it->number() != 0 ? it->number(): 1, true);
    return;

  case Action::nextWindowOfClass:
    cycleWindow(state, true, it->number() != 0 ? it->number(): 1,
                false, false, true, it->string());
    return;

  case Action::prevWindowOfClass:
    cycleWindow(state, false, it->number() != 0 ? it->number(): 1,
                false, false, true, it->string());
    return;

  case Action::nextWindowOfClassOnAllWorkspaces:
    cycleWindow(state, true, it->number() != 0 ? it->number(): 1,
                false, true, true, it->string());
    return;

  case Action::prevWindowOfClassOnAllWorkspaces:
    cycleWindow(state, false, it->number() != 0 ? it->number(): 1,
                false, true, true, it->string());
    return;

  case Action::changeWorkspace:
    changeWorkspace(it->number());
    return;

  case Action::upWorkspace:
    changeWorkspaceVert(-1);
    return;

  case Action::downWorkspace:
    changeWorkspaceVert(1);
    return;

  case Action::leftWorkspace:
    changeWorkspaceHorz(-1);
    return;

  case Action::rightWorkspace:
    changeWorkspaceHorz(1);
    return;

  case Action::execute:
    execCommand(it->string());
    return;

  case Action::showRootMenu:
    _netclient->sendClientMessage(_root, _netclient->xaOpenboxShowRootMenu(),
                                  None);
    return;

  case Action::showWorkspaceMenu:
    _netclient->sendClientMessage(_root, _netclient->xaOpenboxShowWorkspaceMenu(),
                                  None);
    return;

  case Action::toggleGrabs: {
    if (_grabbed) {
      _keybindings->ungrabDefaults(this);
      _grabbed = false;
    } else {
      _keybindings->grabDefaults(this);
      _grabbed = true;
    }
    return;
  }

  default:
    break;
  }

  // these actions require an active window
  if (_active != _clients.end()) {
    XWindow *window = *_active;

    switch (it->type()) {
    case Action::iconify:
      window->iconify();
      return;

    case Action::close:
      window->close();
      return;

    case Action::raise:
      window->raise();
      return;

    case Action::lower:
      window->lower();
      return;

    case Action::sendToWorkspace:
      window->sendTo(it->number());
      return;

    case Action::toggleOmnipresent:
      if (window->desktop() == 0xffffffff)
        window->sendTo(_active_desktop);
      else
        window->sendTo(0xffffffff);
      return;

    case Action::moveWindowUp:
      window->move(0, -(it->number() != 0 ? it->number(): 1));
      return;

    case Action::moveWindowDown:
      window->move(0, it->number() != 0 ? it->number(): 1);
      return;

    case Action::moveWindowLeft:
      window->move(-(it->number() != 0 ? it->number(): 1), 0);
      return;

    case Action::moveWindowRight:
      window->move(it->number() != 0 ? it->number(): 1,0);
      return;

    case Action::resizeWindowWidth:
      window->resizeRel(it->number(), 0);
      return;

    case Action::resizeWindowHeight:
      window->resizeRel(0, it->number());
      return;

    case Action::toggleShade:
      window->shade(! window->shaded());
      return;

    case Action::toggleMaximizeHorizontal:
      window->toggleMaximize(XWindow::Max_Horz);
      return;

    case Action::toggleMaximizeVertical:
      window->toggleMaximize(XWindow::Max_Vert);
      return;

    case Action::toggleMaximizeFull:
      window->toggleMaximize(XWindow::Max_Full);
      return;

    case Action::toggleDecorations:
      window->decorate(! window->decorated());
      return;

    default:
      assert(false);  // unhandled action type!
      break;
    }
  }
}

void ScreenHandler::keyReleaseEvent (const XKeyEvent * const e)
{
  // the only keyrelease event we care about (for now) is when we do window
  // cycling and the modifier is released
  if ( _cycling && nothingIsPressed()) {
    // all modifiers have been released. ungrab the keyboard, move the
    // focused window to the top of the Z-order and raise it
    XUngrabKeyboard(_display, CurrentTime);

    if (_active != _clients.end()) {
      XWindow *w = *_active;
      bool e = _last_active == _active;
      _clients.remove(w);
      _clients.push_front(w);
      _active = _clients.begin();
      if (!e) _last_active = _active;
      w->raise();
    }

    _cycling = false;
  }
}

void ScreenHandler::propertyNotifyEvent(const XPropertyEvent * const e)
{
  if (e->atom == _netclient->numberOfDesktops()) {
    updateNumDesktops();
  } else if (e->atom == _netclient->desktopNames()) {
    updateDesktopNames();
  } else if (e->atom == _netclient->currentDesktop()) {
    updateActiveDesktop();
  } else if (e->atom == _netclient->activeWindow()) {
    updateActiveWindow();
  } else if (e->atom == _netclient->clientList()) {
    updateClientList();
  }
}

void ScreenHandler::updateNumDesktops()
{
  assert(_managed);

  if (! _netclient->readNumberOfDesktops(_root, & _num_desktops))
    _num_desktops = 1;  // assume that there is at least 1 desktop!

}

void ScreenHandler::updateDesktopNames()
{
  assert(_managed);

  if(! _netclient->readDesktopNames(_root, _desktop_names)) {
    _desktop_names.clear();
    return;
  }
  
//  bt::Netwm::UTF8StringList::const_iterator it = _desktop_names.begin(),
//    end = _desktop_names.end();

//  for (; it != end; ++it) {
//    std::cout << BBTOOL << ": " << "name: ->" << *it << "<-\n";
//    char default_name[80];
//    sprintf(default_name, "Workspace %u", _id + 1);
//    the_name = default_name;
//  }
  
}

std::string ScreenHandler::getDesktopName(unsigned int desktopNbr) const {
  
  if (0xFFFFFFFF == desktopNbr)
    return "";

  if (desktopNbr > _desktop_names.size() ) 
    return "error";

  std::string name = _desktop_names[desktopNbr];
  
  return name;

}

void ScreenHandler::updateActiveDesktop()
{
  assert(_managed);

  if (! _netclient->readCurrentDesktop(_root, & _active_desktop))
    _active_desktop = 0;  // there must be at least one desktop, and it must
  // be the current one

}

void ScreenHandler::updateActiveWindow()
{
  assert(_managed);

  Window a = None;
  _netclient->getValue(_root, _netclient->activeWindow(), XA_WINDOW, a);

  if ( None == a ) {
    return;
  }

  WindowList::iterator it, end = _clients.end();
  for (it = _clients.begin(); it != end; ++it) {
    if ( (*it)->window() == a) {
      if ( (*it)->getScreenNumber() != _screenNumber )
        return;
      break;
    }
  }

  _active = it;

  if (_active != end) {
    /* if we're not cycling and a window gets focus, add it to the top of the
     * cycle stack.
     */

    if ( !_cycling) {
      XWindow *win = *_active;
      _clients.remove(win);
      _clients.push_front(win);
      _active = _clients.begin();

      _last_active = _active;

      if ( _debug )
        cout <<BBTOOL << ": " << "active window now: [" <<(*_active)->title() <<"]" <<endl;
    }

  }

}

void ScreenHandler::updateClientList()
{

  assert(_managed);

  WindowList::iterator insert_point = _active;
  if (insert_point != _clients.end())
    ++insert_point; // get to the item client the focused client

  // get the client list from the root window
  Netclient::WindowList rootclients;
  unsigned long num = (unsigned) -1;

  if ( ! _netclient->readClientList(_root, rootclients) ) {
    cerr << BBTOOL << ": " << "couldn't get client list from WM.\n";
    num = 0;
  } else {
    num = rootclients.size();
  }

  WindowList::iterator it;
  const WindowList::iterator end = _clients.end();
  unsigned long i;

  for (i = 0; i < num; ++i) {
    for (it = _clients.begin(); it != end; ++it)
      if (**it == rootclients[i])
        break;
    if (it == end) {  // didn't already exist
      if (careAboutWindow(rootclients[i])) {
        XWindow * wTmp = new XWindow( rootclients[i], _netclient, _screenInfo , *_keyClient );
        _clients.insert(insert_point, wTmp);
      }
    }
  }

  // remove clients that no longer exist (that belong to this screen)
  for (it = _clients.begin(); it != end;) {
    WindowList::iterator it2 = it;
    ++it;

    // is on another screen?
    if ((*it2)->getScreenNumber() != _screenNumber)
      continue;

    for (i = 0; i < num; ++i)
      if (**it2 == rootclients[i])
        break;
    if (i == num)  { // no longer exists
      // watch for the active and last-active window
      if (it2 == _active)
        _active = _clients.end();
      if (it2 == _last_active)
        _last_active = _clients.end();
      delete *it2;
      _clients.erase(it2);
    }
  }

}


// do we care about this window as a client?
bool ScreenHandler::careAboutWindow(Window window) const
{
  assert(_managed);

  Atom type;
  if (! _netclient->getValue(window, _netclient->wmWindowType(), XA_ATOM,
                             type)) {
    return True;
  }

  if (type == _netclient->wmWindowTypeDock() ||
      type == _netclient->wmWindowTypeMenu() ) {
    return False;
  } else {
    return True;
  }
}

XWindow * ScreenHandler::findWindow(Window window) const {
  assert(_managed);

  WindowList::const_iterator it, end = _clients.end();
  for (it = _clients.begin(); it != end; ++it)
    if (**it == window)
      break;
  if(it == end)
    return 0;
  return *it;
}

void ScreenHandler::execCommand(const string &cmd) const {
  pid_t pid;
  if ((pid = fork()) == 0) {
    // disconnect the child from this session and the tty
    if (setsid() == -1) {
      cout << BBTOOL << ": " << "warning: could not start a new process group\n";
      perror("setsid");
    }

    // make the command run on the correct screen
    if (putenv(const_cast<char*>(_screenInfo.displayString().c_str()))) {
      cout << BBTOOL << ": " << "warning: couldn't set environment variable 'DISPLAY'\n";
      perror("putenv()");
    }
    execl("/bin/sh", "sh", "-c", cmd.c_str(), NULL);
    exit(-1);
  } else if (pid == -1) {
    cout << BBTOOL << ": " << ": Could not fork a process for executing a command\n";
  }
}

WindowList ScreenHandler::getCycleWindowList(unsigned int state, const bool forward,
					     const int increment, const bool allscreens,
					     const bool alldesktops, const bool sameclass,
					     const string &cn)
{
  assert(_managed);
  assert(increment > 0);

  WindowList theList;

  if (_clients.empty()) return theList;

  string classname(cn);
  if (sameclass && classname.empty() && _active != _clients.end())
    classname = (*_active)->appClass();


  WindowList::const_iterator it = _clients.begin();
  const WindowList::const_iterator end = _clients.end();

  for (; it != end; ++it) {
    XWindow *t = *it;

    // determine if this window is invalid for cycling to
    if (t->iconic()) continue;
    if (! allscreens && t->getScreenNumber() != _screenNumber) continue;
    if (! alldesktops && ! (t->desktop() == _active_desktop ||
			    t->desktop() == 0xffffffff)) continue;
    if (sameclass && ! classname.empty() &&
	t->appClass() != classname) continue;
    if (! t->canFocus()) continue;
    if (t->skipPager()) continue;

    // found a focusable window
    theList.push_back(t);
  }

  return theList;

}

void ScreenHandler::cycleWindow(unsigned int state, const bool forward,
				const int increment, const bool allscreens,
				const bool alldesktops, const bool sameclass,
				const string &cn)
{
  assert(_managed);
  assert(increment > 0);

  if (_clients.empty()) return;

  // if our user wants the window cycling menu to show (who wouldn't!!
  //  =:) ) and if it's not already showing...
  if ( _show_cycle_menu && ! _windowmenu->isVisible() ) {
    if (_debug)
      std::cout << BBTOOL << ": " << "ScreenHandler: menu not visible. loading and showing..." << std::endl;

    _cycling = true;
    WindowList theList = getCycleWindowList(state, forward, increment,
					    allscreens, alldesktops,
					    sameclass, cn);
    // can't show the window list if there's not even one window  =:)
    if (theList.size() >= 1)
      _windowmenu->showCycleMenu(theList);

    return;

  }

  string classname(cn);
  if (sameclass && classname.empty() && _active != _clients.end())
    classname = (*_active)->appClass();

  WindowList::const_iterator target = _active,
    begin = _clients.begin(),
    end = _clients.end();

  XWindow *t = 0;

  for (int x = 0; x < increment; ++x) {
    while (1) {
      if (forward) {
	if (target == end)
	  target = begin;
	else
	  ++target;
      } else {
	if (target == begin)
	  target = end;
	else
	  --target;
      }

      // must be no window to focus
      if (target == _active)
	return;

      // start back at the beginning of the loop
      if (target == end)
	continue;

      // determine if this window is invalid for cycling to
      t = *target;
      if (t->iconic()) continue;
      if (! allscreens && t->getScreenNumber() != _screenNumber) continue;
      if (! alldesktops && ! (t->desktop() == _active_desktop ||
			      t->desktop() == 0xffffffff)) continue;
      if (sameclass && ! classname.empty() &&
	  t->appClass() != classname) continue;
      if (! t->canFocus()) continue;
      if (t->skipPager()) continue;

      // found a good window so break out of the while, and perhaps continue
      // with the for loop
      break;
    }
  }

  // phew. we found the window, so focus it.
  if ( state) {
    if (!_cycling) {
      // grab keyboard so we can intercept KeyReleases from it
      XGrabKeyboard(_display, _root, True, GrabModeAsync,
		    GrabModeAsync, CurrentTime);
      _cycling = true;
    }

    // if the window is on another desktop, we can't use XSetInputFocus, since
    // it doesn't imply a workspace change.
    if ( t->desktop() != _active_desktop &&
	 t->desktop() != 0xffffffff)
      t->focus(); // raise
    else
      t->focus(false); // don't raise
  }
  else {
    t->focus();
  }
}

void ScreenHandler::focusWindow(const XWindow * win) {

  // if the window our little user has selected is on a different 
  // workspace, go there first
  if ( ! win->isSticky() && (_active_desktop != win->desktop() ) ) {
    changeWorkspace(win->desktop() );
  }

  win->focus(true);

}

void ScreenHandler::cycleWorkspace(const bool forward, const int increment,
				   const bool loop) const {
  assert(_managed);
  assert(increment > 0);

  unsigned int destination = _active_desktop;

  for (int x = 0; x < increment; ++x) {
    if (forward) {
      if (destination < _num_desktops - 1)
	++destination;
      else if (loop)
	destination = 0;
    } else {
      if (destination > 0)
	--destination;
      else if (loop)
	destination = _num_desktops - 1;
    }
  }

  if (destination != _active_desktop)
    changeWorkspace(destination);
}


void ScreenHandler::changeWorkspace(const int num) const {
  assert(_managed);

  _netclient->sendClientMessage(_root, _netclient->currentDesktop(), _root, num);
}

void ScreenHandler::changeWorkspaceVert(const int num) const {
  assert(_managed);
  int width  = _workspace_columns;
  int total = (signed)_num_desktops;
  int n = (signed)_active_desktop;
  int wnum = 0;

  // if the # of rows is greater than the # of desktops or <= 0, or
  // if we're not dealing with a rectangle here, return (invalid condition)
  if ( width > total || width <= 0 ||
       ( total % width ) != 0 )
    return;

  bool moveUp = (num < 0);
  if (moveUp) { // we go up...
    wnum = (n < width) // if we're on the first row
      ? n + (total - width) // go to the same position in the last row
      : n - width; // else, just go up one row

  } else { // we go down...
    wnum = (n < (total - width)) // if we're not on the last row
      ? n + width // go down one row
      : n - (total - width); // else go to the same position in the
			     // first row

  }
  changeWorkspace(wnum);
}

void ScreenHandler::changeWorkspaceHorz(const int num) const {
  assert(_managed);
  int width = _workspace_columns;
  int total = (signed)_num_desktops;
  int n = (signed)_active_desktop;
  int wnum = 0;

  int curx = (n % width);

  // if the # of rows is greater than the # of desktops or <= 0, or
  // if we're not dealing with a rectangle here, return (invalid condition)
  if ( width > total || width <= 0 ||
       ( total % width ) != 0 )
    return;

  bool moveLeft = (num < 0);
  if (moveLeft) {
    wnum = (curx % width == 0) // if we're on the left edge already
      ? n + (width -1) // move to the far right side
      : n -1; // else, just move once left
  } else {
    wnum = (curx < width -1) // if we're not on the right side
      ? n + 1 // move once to the right
      : n - (width -1); // else move all the way to the left
  }
  changeWorkspace(wnum);
}

bool ScreenHandler::nothingIsPressed(void) const
{
  char keys[32];
  XQueryKeymap(_display, keys);

  for (int i = 0; i < 32; ++i) {
    if (keys[i] != 0)
      return false;
  }

  return true;
}

const XWindow *ScreenHandler::lastActiveWindow() const {
  if (_last_active != _clients.end())
    return *_last_active;

  // find a window if one exists
  WindowList::const_iterator it, end = _clients.end();
  for (it = _clients.begin(); it != end; ++it)
    if ((*it)->getScreenNumber() == _screenNumber && ! (*it)->iconic() &&
        (*it)->canFocus() &&
        ((*it)->desktop() == 0xffffffff ||
         (*it)->desktop() == _active_desktop))
      return *it;

  // no windows on this screen
  return 0;
}

void ScreenHandler::p()
{
  cout << BBTOOL << ": " << "\nNOW LISTING CLIENTS!!!" << endl;

  WindowList::const_iterator it = _clients.begin();
  const WindowList::const_iterator end = _clients.end();

  for (; it != end; ++it)
    cout << BBTOOL << ": " << "desktop: ["
         << (*it)->desktop()
         << "], window: [" << (*it)->title() << "]" << endl;


}


