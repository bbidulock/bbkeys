// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// window.cc for Epistrophy - a key handler for NETWM/EWMH window managers.
// Copyright (c) 2002 - 2002 Ben Jansens <ben at orodu.net>
//
// Modified for use and inclusion in bbkeys by Jason 'vanRijn' Kasper
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

#include <iostream>

using std::cout;
using std::endl;

#include "window.hh"

// defined by black/openbox
const unsigned long XWindow::PropBlackboxAttributesElements;
const unsigned long XWindow::AttribDecoration;
const unsigned long XWindow::DecorNone;
const unsigned long XWindow::DecorNormal;


XWindow::XWindow(Window window, Netclient * netclient,
                 const bt::ScreenInfo & screenInfo, bt::Application & app)
  : _window(window), _netclient(netclient), _screenInfo(screenInfo),
    _app(app) {

  _unmapped = false;

  _display=_screenInfo.display().XDisplay();
  _root = _screenInfo.rootWindow();

  XSelectInput(_display, _window,
               PropertyChangeMask | StructureNotifyMask);

  // add an event handler for our window
  _app.insertEventHandler(_window, this);  

  updateBlackboxAttributes();
  updateNormalHints();
  updateWMHints();
  updateDimensions();
  updateState();
  updateDesktop();
  updateTitle();
  updateClass();

}


XWindow::~XWindow() {
  if (! _unmapped)
    XSelectInput(_display, _window, None);

  // tell our main app about our death
  _app.removeEventHandler( _window );
}

// Window configure (size, position, stacking, etc.).
void XWindow::configureNotifyEvent(const XConfigureEvent * const e) {
  updateDimensions();
}

// Window property changed/added/deleted.
void XWindow::propertyNotifyEvent(const XPropertyEvent * const e) {
  
  if (e->atom == XA_WM_NORMAL_HINTS)
    updateNormalHints();
  else if (e->atom == XA_WM_HINTS)
    updateWMHints();
  else if (e->atom == _netclient->xaBlackboxAttributes())
    updateBlackboxAttributes();  
  else if (e->atom == _netclient->wmState() )
    updateState();
  else if (e->atom == _netclient->wmDesktop() )
    updateDesktop();
  else if (e->atom == _netclient->wmName() ||
           e->atom == _netclient->xaWmName() )
    updateTitle();
  else if (e->atom == _netclient->xaWmClass() )
    updateClass();
}

// Window hidden.
void XWindow::unmapNotifyEvent(const XUnmapEvent * const e) {
  _unmapped = true;
}

// Window destroyed.
void XWindow::destroyNotifyEvent(const XDestroyWindowEvent * const e) {
  _unmapped = true;
}

void XWindow::updateDimensions(const XConfigureEvent * const e) {
  _rect.setRect(e->x, e->y, e->width, e->height);
}
  
void XWindow::updateDimensions() {

  XWindowAttributes win_attributes;
  Window junkwin;
  int rx, ry;

  _rect.setRect(0,0,1,1);

  if (! XGetWindowAttributes(_display, _window, &win_attributes)) {
    std::cerr << "updateDimensions. couldn't get what I needed, so setting to ridiculously wrong values.\n";
    return;
  }

  XTranslateCoordinates (_display, _window, win_attributes.root, 
                         -win_attributes.border_width,
                         -win_attributes.border_width,
                         &rx, &ry, &junkwin);
  _rect.setRect(rx, ry, win_attributes.width, win_attributes.height);
  
#if DEBUG
  std::cout << endl <<
    "absolute upper-left x: " << rx << endl <<
    "absolute upper-left y: " << ry << endl <<
    "relative upper-left x: " << win_attributes.x << endl <<
    "relative upper-left y: " << win_attributes.y << endl;
#endif

}

void XWindow::updateBlackboxAttributes() {
  unsigned long *data;
  unsigned long num = PropBlackboxAttributesElements;

  _decorated = true;

  if (_netclient->getValue(_window,
                           _netclient->xaBlackboxAttributes(),
                           _netclient->xaBlackboxAttributes(), 
                           num, &data)) {
    if (num == PropBlackboxAttributesElements)
      if (data[0] & AttribDecoration)
        _decorated = (data[4] != DecorNone);
    delete data;
  }
}

void XWindow::updateNormalHints() {
  XSizeHints size;
  long ret;

  // defaults
  _gravity = NorthWestGravity;
  _inc_x = _inc_y = 1;
  _base_x = _base_y = 0;
  
  if (XGetWMNormalHints(_display, _window, &size, &ret)) {
    if (size.flags & PWinGravity)
      _gravity = size.win_gravity;
    if (size.flags & PBaseSize) {
      _base_x = size.base_width;
      _base_y = size.base_height;
    }
    if (size.flags & PResizeInc) {
      _inc_x = size.width_inc;
      _inc_y = size.height_inc;
    }
  }
}


void XWindow::updateWMHints() {
  XWMHints *hints;

  // assume a window takes input if it doesnt specify
  _can_focus = True;
  
  if ((hints = XGetWMHints(_display, _window)) != NULL) {
    if (hints->flags & InputHint)
      _can_focus = hints->input;
    XFree(hints);
  }
}


void XWindow::updateState() {
  // set the defaults
  _shaded = _iconic = _max_vert = _max_horz = false;
  
  unsigned long num = (unsigned) -1;
  Atom *state;
  if (! _netclient->getValue(_window, _netclient->wmState(), XA_ATOM,
                             num, &state))
    return;
  for (unsigned long i = 0; i < num; ++i) {
    if (state[i] == _netclient->wmStateMaximizedVert())
      _max_vert = true;
    if (state[i] == _netclient->wmStateMaximizedHorz())
      _max_horz = true;
    if (state[i] == _netclient->wmStateShaded())
      _shaded = true;
    if (state[i] == _netclient->wmStateHidden())
      _iconic = true;
  }

  delete [] state;
}


void XWindow::updateDesktop() {
  unsigned long d = 0ul;
  if (! _netclient->getValue(_window, _netclient->wmDesktop(), XA_CARDINAL, d))
    d = 0ul;

  _desktop = static_cast<unsigned int>(d);
}


void XWindow::updateTitle() {
  _title = "";
  
  // try netwm
  if (! _netclient->getValue(_window, _netclient->wmName(),
                             Netclient::utf8, _title)) {
    // try old x stuff
    _netclient->getValue(_window, XA_WM_NAME, Netclient::ansi, _title);
  }
  
  if (_title.empty())
    _title = "Unnamed";

}


void XWindow::updateClass() {
  // set the defaults
  _app_name = _app_class = "";

  Netclient::StringVect v;
  unsigned long num = 2;

  if (! _netclient->getValue(_window, XA_WM_CLASS, Netclient::ansi, num, v))
    return;

  if (num > 0) _app_name = v[0];
  if (num > 1) _app_class = v[1];
}



void XWindow::shade(const bool sh) const {
  _netclient->sendClientMessage(_root, _netclient->wmState(),
                                _window, (sh ? 1 : 0),
                                _netclient->wmStateShaded());
}

void XWindow::close() const {
  _netclient->sendClientMessage(_root, _netclient->closeWindow(),
                                _window);
}


void XWindow::raise() const {
  XRaiseWindow(_display, _window);
}

void XWindow::lower() const {
  XLowerWindow(_display, _window);
}

void XWindow::iconify() const {
  _netclient->sendClientMessage(_root, _netclient->xaWmChangeState(),
                                _window, IconicState);
}

void XWindow::focus(bool raise) const {
  // this will cause the window to be uniconified also

  if (raise) {
    _netclient->sendClientMessage(_root, _netclient->activeWindow(),
                                  _window);
  } else {
    XSetInputFocus(_display, _window, None, CurrentTime);
  }
}

void XWindow::decorate(bool d) const {
  _netclient->sendClientMessage(_root,
                                _netclient->xaBlackboxChangeAttributes(),
                                _window, AttribDecoration,
                                0, 0, 0, (d ? DecorNormal : DecorNone));
}

void XWindow::sendTo(unsigned int dest) const {
  _netclient->sendClientMessage(_root, _netclient->wmDesktop(),
                                _window, dest);
}


void XWindow::move(int x, int y) const {

  XWindowAttributes win_attributes;
  Window junkwin;
  int rx, ry;


  if (! XGetWindowAttributes(_display, _window, &win_attributes)) {
    std::cerr << "move: couldn't get what I needed. not able to move, sorry.\n";
    return;
  }

  XTranslateCoordinates (_display, _window, win_attributes.root, 
                         -win_attributes.border_width,
                         -win_attributes.border_width,
                         &rx, &ry, &junkwin);
      
  Status status;
  int xright, ybelow;
  int dw = _screenInfo.width(), dh = _screenInfo.height();

  
  /* find our window manager frame, if any */
  Window wmframe = _window;

  while (True) {
    Window root, parent;
    Window *childlist;
    unsigned int ujunk;

    status = XQueryTree(_display, wmframe, &root, &parent, &childlist, &ujunk);
    if (parent == root || !parent || !status)
      break;
    wmframe = parent;
    if (status && childlist)
      XFree((char *)childlist);
  }

  if (wmframe != _window) {
    /* WM reparented, so find edges of the frame */
    /* Only works for ICCCM-compliant WMs, and then only if the
       window has corner gravity.  We would need to know the original width
       of the window to correctly handle the other gravities. */

    XWindowAttributes frame_attr;

    if (!XGetWindowAttributes(_display, wmframe, &frame_attr)) {
      std::cerr << "updateDimensions. error. can't get frame attributes.\n";
    }
    
    switch (_gravity) {
    case NorthWestGravity: case SouthWestGravity:
    case NorthEastGravity: case SouthEastGravity:
    case WestGravity:
      rx = frame_attr.x;
    }
    switch (_gravity) {
    case NorthWestGravity: case SouthWestGravity:
    case NorthEastGravity: case SouthEastGravity:
    case EastGravity:
      xright = dw - frame_attr.x - frame_attr.width -
        2*frame_attr.border_width;
    }
    switch (_gravity) {
    case NorthWestGravity: case SouthWestGravity:
    case NorthEastGravity: case SouthEastGravity:
    case NorthGravity:
      ry = frame_attr.y;
    }
    switch (_gravity) {
    case NorthWestGravity: case SouthWestGravity:
    case NorthEastGravity: case SouthEastGravity:
    case SouthGravity:
      ybelow = dh - frame_attr.y - frame_attr.height -
        2*frame_attr.border_width;
    }
  }

  int destx = rx +x, desty = ry +y;

#if DEBUG
  std::cout<< "moving window to: +" << destx << "+" << desty << endl;
#endif
  
  XMoveWindow(_display, _window, destx, desty);

}


void XWindow::resizeRel(int dwidth, int dheight) const {
  // resize in increments if requested by the window
  unsigned int width = _rect.width(), height = _rect.height();
  
  unsigned int wdest = width + (dwidth * _inc_x) / _inc_x * _inc_x + _base_x;
  unsigned int hdest = height + (dheight * _inc_y) / _inc_y * _inc_y + _base_y;

  XResizeWindow(_display, _window, wdest, hdest);
}


void XWindow::resizeAbs(unsigned int width, unsigned int height) const {
  // resize in increments if requested by the window

  unsigned int wdest = width / _inc_x * _inc_x + _base_x;
  unsigned int hdest = height / _inc_y * _inc_y + _base_y;

  if (width > wdest) {
    while (width > wdest)
      wdest += _inc_x;
  } else {
    while (width < wdest)
      wdest -= _inc_x;
  }
  if (height > hdest) {
    while (height > hdest)
      hdest += _inc_y;
  } else {
    while (height < hdest)
      hdest -= _inc_y;
  }
  
  XResizeWindow(_display, _window, wdest, hdest);
}


void XWindow::toggleMaximize(Max max) const {
  switch (max) {
  case Max_Full:
    _netclient->
      sendClientMessage(_root, _netclient->wmState(),
                        _window, (_max_vert == _max_horz ? 2 : 1),
                        _netclient->wmStateMaximizedHorz(),
                        _netclient->wmStateMaximizedVert());
    break;

  case Max_Horz:
    _netclient->
      sendClientMessage(_root, _netclient->wmState(),
                        _window, 2,
                        _netclient->wmStateMaximizedHorz());
    break;

  case Max_Vert:
    _netclient->
      sendClientMessage(_root, _netclient->wmState(),
                        _window, 2,
                        _netclient->wmStateMaximizedVert());
    break;
    
  case Max_None:
    assert(false);  // you should not do this. it is pointless and probly a bug
    break;
  }
}


void XWindow::maximize(Max max) const {
  switch (max) {
  case Max_None:
    _netclient->
      sendClientMessage(_root, _netclient->wmState(),
                        _window, 0,
                        _netclient->wmStateMaximizedHorz(),
                        _netclient->wmStateMaximizedVert());
    break;

  case Max_Full:
    _netclient->
      sendClientMessage(_root, _netclient->wmState(),
                        _window, 1,
                        _netclient->wmStateMaximizedHorz(),
                        _netclient->wmStateMaximizedVert());
    break;

  case Max_Horz:
    _netclient->
      sendClientMessage(_root, _netclient->wmState(),
                        _window, 1,
                        _netclient->wmStateMaximizedHorz());
    break;

  case Max_Vert:
    _netclient->
      sendClientMessage(_root, _netclient->wmState(),
                        _window, 1,
                        _netclient->wmStateMaximizedVert());
    break;
  }
}

