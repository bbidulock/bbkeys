// -*- mode: C++; indent-tabs-mode: nil; -*-
// window.hh for Epistrophy - a key handler for NETWM/EWMH window managers.
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

#ifndef   __window_hh
#define   __window_hh

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
}

#include <list>
#include <string>

// blackbox lib....
#include "Application.hh"
#include "Util.hh"
#include "Display.hh"
#include "EventHandler.hh"

#include "Netclient.h"
#include "version.h"

class XWindow : public bt::EventHandler {
public:
  enum Max {
    Max_None,
    Max_Horz,
    Max_Vert,
    Max_Full
  };

private:
  // defined by black/openbox
  static const unsigned long PropBlackboxAttributesElements = 9;
  static const unsigned long AttribDecoration = 1 << 6;
  static const unsigned long DecorNone = 0;
  static const unsigned long DecorNormal = 2;

  Window _window;
  Display * _display;
  Window _root;
  Netclient * _netclient;
  const bt::ScreenInfo & _screenInfo;
  bt::Application & _app;

  unsigned int _desktop;
  bt::ustring _title;
  std::string _app_name;
  std::string _app_class;
  bt::Rect _rect;
  int _inc_x, _inc_y; // resize increments
  int _base_x, _base_y; // base size
  int _gravity;
  bool _can_focus;

  // states
  bool _shaded;
  bool _iconic;
  bool _skip_pager;
  bool _max_vert;
  bool _max_horz;
  bool _decorated;

  bool _unmapped;

  void updateDimensions();
  void updateDimensions(const XConfigureEvent * const e);
  void updateBlackboxAttributes();
  void updateNormalHints();
  void updateWMHints();
  void updateState();
  void updateDesktop();
  void updateTitle();
  void updateClass();
  void updateMotifHints();

  // EventHandler methods....
  
  // Window configure (size, position, stacking, etc.).
  void configureNotifyEvent(const XConfigureEvent * const e);

  // Window property changed/added/deleted.
  void propertyNotifyEvent(const XPropertyEvent * const e);

  // Window hidden.
  void unmapNotifyEvent(const XUnmapEvent * const e);

  // Window destroyed.
  void destroyNotifyEvent(const XDestroyWindowEvent * const e);

public:
  XWindow(Window window, Netclient * netclient, 
          const bt::ScreenInfo & screenInfo, bt::Application & app);
  virtual ~XWindow();

  inline unsigned int getScreenNumber() const { return _screenInfo.screenNumber(); }
  inline Window window() const { return _window; }
  
  inline unsigned int desktop() const { return _desktop; }
  inline const bt::ustring &title() const { return _title; }
  inline const std::string &appName() const { return _app_name; }
  inline const std::string &appClass() const { return _app_class; }
  inline bool canFocus() const { return _can_focus; }
  inline bool isSticky() const { return 0xFFFFFFFF == _desktop; }
  
  inline bool shaded() const { return _shaded; }
  inline bool iconic() const { return _iconic; }
  inline bool skipPager() const { return _skip_pager; }
  inline bool maxVert() const { return _max_vert; }
  inline bool maxHorz() const { return _max_horz; }
  inline bool decorated() const { return _decorated; }
  inline const bt::Rect &area() const { return _rect; }
  inline unsigned int x() const { return _rect.x(); }
  inline unsigned int y() const { return _rect.y(); }
  inline unsigned int width() const { return _rect.width(); }
  inline unsigned int height() const { return _rect.height(); }

  void shade(const bool sh) const;
  void close() const;
  void raise() const;
  void lower() const;
  void iconify() const;
  void focus(bool raise = true) const;
  void decorate(bool d) const;
  void sendTo(unsigned int dest) const;
  void move(int x, int y) const;
  void resizeRel(int dwidth, int dheight) const;
  void resizeAbs(unsigned int width, unsigned int height) const;
  void toggleMaximize(Max max) const; // i hate toggle functions
  void maximize(Max max) const;

  bool operator == (const XWindow &w) const { return w._window == _window; }
  bool operator == (const Window &w) const { return w == _window; }
};

typedef std::list<XWindow *> WindowList;

#endif // __window_hh
