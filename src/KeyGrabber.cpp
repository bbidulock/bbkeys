// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- KeyGrabber.cpp --
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

#include <iostream>

#include "version.h"
#include "KeyGrabber.h"
//--------------------------------------------------------
// Constructor/Destructor 
//--------------------------------------------------------
KeyGrabber::KeyGrabber (Display * display, unsigned int numlockMask, unsigned int scrolllockMask)
  : _display(display), _numlockMask(numlockMask), _scrolllockMask(scrolllockMask)
{
}
KeyGrabber::~KeyGrabber ()
{
}
//--------------------------------------------------------
// grab 
//--------------------------------------------------------
bool KeyGrabber::grab (const KeyCode & keyCode,
                       const unsigned int & modifierMask,  Window window)
{
  // int ret = 
  XGrabKey(_display, keyCode, modifierMask,
           window, True, GrabModeAsync, GrabModeAsync);
  XGrabKey(_display, keyCode, 
           modifierMask|LockMask,
           window, True, GrabModeAsync, GrabModeAsync);
  XGrabKey(_display, keyCode, 
           modifierMask|_scrolllockMask,
           window, True, GrabModeAsync, GrabModeAsync);
  XGrabKey(_display, keyCode, 
           modifierMask|_numlockMask,
           window, True, GrabModeAsync, GrabModeAsync);
    
  XGrabKey(_display, keyCode, 
           modifierMask|LockMask|_scrolllockMask,
           window, True, GrabModeAsync, GrabModeAsync);
  XGrabKey(_display, keyCode, 
           modifierMask|_scrolllockMask|_numlockMask,
           window, True, GrabModeAsync, GrabModeAsync);
  XGrabKey(_display, keyCode, 
           modifierMask|_numlockMask|LockMask,
           window, True, GrabModeAsync, GrabModeAsync);
    
  XGrabKey(_display, keyCode, 
           modifierMask|_numlockMask|LockMask|_scrolllockMask,
           window, True, GrabModeAsync, GrabModeAsync);

  // Um.  Isn't there SOME way we can tell if a grab worked???
  // return (ret == Success);
  return true;
}
//--------------------------------------------------------
// ungrab 
//--------------------------------------------------------
bool KeyGrabber::ungrab (const KeyCode & keyCode,
                         const unsigned int & modifierMask,  Window window)
{
  XUngrabKey(_display, keyCode, modifierMask, window);
  XUngrabKey(_display, keyCode, modifierMask|LockMask, window);
  XUngrabKey(_display, keyCode, modifierMask|_scrolllockMask, window);
  XUngrabKey(_display, keyCode, modifierMask|_numlockMask, window);

  XUngrabKey(_display, keyCode, modifierMask|LockMask|_scrolllockMask, window);
  XUngrabKey(_display, keyCode, modifierMask|_scrolllockMask|_numlockMask, window);
  XUngrabKey(_display, keyCode, modifierMask|_numlockMask|LockMask, window);
  XUngrabKey(_display, keyCode, modifierMask|_numlockMask|LockMask|
             _scrolllockMask, window);  
 
  return true;
}
//--------------------------------------------------------
// ungrabAll 
//--------------------------------------------------------
bool KeyGrabber::ungrabAll (Window  window)
{

  XUngrabKey(_display, AnyKey, AnyModifier, window);
 
  return true;
}
