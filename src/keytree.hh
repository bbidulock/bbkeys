// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// keytree.hh for Epistrophy - a key handler for NETWM/EWMH window managers.
// Copyright (c) 2002 - 2002 Ben Jansens <ben at orodu.net>
//
// Adapted slightly for inclusion in bbkeys by Jason 'vanRijn' Kasper
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

#ifndef _keytree_hh
#define _keytree_hh

extern "C" {
#include <X11/Xlib.h>
}

#include <list>
#include "actions.hh"
#include "ScreenHandler.h"

class ScreenHandler;

struct keynode; // forward declaration
typedef std::list<keynode *> ChildList;

struct keynode {
  Action *action;
  keynode *parent;
  ChildList children;
};

class keytree {
public:
  keytree(Display * display);
  virtual ~keytree();

  Action * getAction(const XKeyEvent  * const e, unsigned int &, ScreenHandler * screen);
  void unloadBindings();
  inline void showTree() {showTree(_head); }
  inline bool isLeaf (keynode *node) { return node->children.empty(); }
  void grabDefaults(ScreenHandler * screen);
  void ungrabDefaults(ScreenHandler * screen);  
  void showTree(keynode *);

private:
  friend class KeyClient;
  void grabChildren(keynode *, ScreenHandler * screen);
  void ungrabChildren(keynode *, ScreenHandler * screen);
  
  void addAction(Action::ActionType, unsigned int, std::string, std::string);
  void advanceOnNewNode();
  void retract();
  void setCurrentNodeProps(Action::ActionType, unsigned int, std::string, std::string);
  void initialize();

  void reset()
  { _current = _head; }

  void clearTree(keynode *);

  keynode *_head;
  keynode *_current;
  Display *_display;
};

#endif // _keytree_hh
