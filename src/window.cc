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

void XWindow::updateDimensions() {
  Window root, child;
  int x, y;
  unsigned int w, h, b, d;

  if (XGetGeometry(_display, _window, &root, &x, &y, &w, &h,
                   &b, &d) &&
      XTranslateCoordinates(_display, _window, root, x, y,
                            &x, &y, &child))
    _rect.setRect(x, y, w, h);
  else
    _rect.setRect(0, 0, 1, 1);
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
  // get the window's decoration sizes (margins)
  bt::Netwm::Strut margins;
  Window win = _window, parent, root, last = None;
  Window *children = 0;
  unsigned int nchildren;
  XWindowAttributes wattr;
  
  while (XQueryTree(_display, win, &root, &parent, &children,
                    &nchildren)) {
    if (children && nchildren > 0)
      XFree(children); // don't care about the children

    if (! parent) // no parent!?
      return;

    // if the parent window is the root window, stop here
    if (parent == root)
      break;

    last = win;
    win = parent;
  }

  if (! (XTranslateCoordinates(_display, last, win, 0, 0,
                               (int *) &margins.left,
                               (int *) &margins.top,
                               &parent) &&
         XGetWindowAttributes(_display, win, &wattr)))
    return;

  margins.right = wattr.width - _rect.width() - margins.left;
  margins.bottom = wattr.height - _rect.height() - margins.top;

  margins.left += wattr.border_width;
  margins.right += wattr.border_width;
  margins.top += wattr.border_width;
  margins.bottom += wattr.border_width;

  // this makes things work. why? i don't know. but you need them.
  margins.right -= 2;
  margins.bottom -= 2;
  
  // find the frame's reference position based on the window's gravity
  switch (_gravity) {
  case NorthWestGravity:
    x -= margins.left;
    y -= margins.top;
    break;
  case NorthGravity:
    x += (margins.left + margins.right) / 2;
    y -= margins.top;
    break;
  case NorthEastGravity:
    x += margins.right;
    y -= margins.top;
  case WestGravity:
    x -= margins.left;
    y += (margins.top + margins.bottom) / 2;
    break;
  case CenterGravity:
    x += (margins.left + margins.right) / 2;
    y += (margins.top + margins.bottom) / 2;
    break;
  case EastGravity:
    x += margins.right;
    y += (margins.top + margins.bottom) / 2;
  case SouthWestGravity:
    x -= margins.left;
    y += margins.bottom;
    break;
  case SouthGravity:
    x += (margins.left + margins.right) / 2;
    y += margins.bottom;
    break;
  case SouthEastGravity:
    x += margins.right;
    y += margins.bottom;
    break;
  default:
    break;
  }
  
  XMoveWindow(_display, _window, x, y);
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

