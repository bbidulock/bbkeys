// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// keytree.cc for Epistrophy - a key handler for NETWM/EWMH window managers.
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

#include "keytree.hh"

#include <string>
#include <iostream>

using std::string;

keytree::keytree(Display *display): _display(display)
{
  _head = new keynode;
  _head->parent = NULL;
  _head->action = NULL; // head's action is always NULL
  _current = _head;
  // for complete initialization, initialize() has to be called as well. We
  // call initialize() when we are certain that the config object (which the
  // timer uses) has been fully initialized. (see parser::parse())
}

keytree::~keytree()
{
  clearTree(_head);
}

void keytree::unloadBindings()
{
  ChildList::iterator it, end = _head->children.end();
  for (it = _head->children.begin(); it != end; ++it)
    clearTree(*it);

  _head->children.clear();
  reset();
}

void keytree::clearTree(keynode *node)
{
  if (!node)
    return;

  ChildList::iterator it, end = node->children.end();
  for (it = node->children.begin(); it != end; ++it)
    clearTree(*it);

  node->children.clear();

  if (node->action)
    delete node->action;
  delete node;
  node = NULL;
}

void keytree::grabDefaults(ScreenHandler *scr)
{
  grabChildren(_head, scr);
}

void keytree::ungrabDefaults(ScreenHandler *scr)
{
  Action *act;

  ChildList::const_iterator it, end = _head->children.end();
  for (it = _head->children.begin(); it != end; ++it) {
    act = (*it)->action;
    if (act && act->type() != Action::toggleGrabs)
      scr->ungrabKey(act->keycode(), act->modifierMask());
  }
}

void keytree::grabChildren(keynode *node, ScreenHandler *scr)
{
  Action *act;

  ChildList::const_iterator it, end = node->children.end();
  for (it = node->children.begin(); it != end; ++it) {
    act = (*it)->action;
    if (act)
      scr->grabKey(act->keycode(), act->modifierMask());
  }
}

void keytree::ungrabChildren(keynode *node, ScreenHandler *scr)
{
  ChildList::const_iterator head_it, head_end = _head->children.end();
  ChildList::const_iterator it, end = node->children.end();
  bool ungrab = true;
 
  // when ungrabbing children, make sure that we don't ungrab any topmost keys
  // (children of the head node) This would render those topmost keys useless.
  // Topmost keys are _never_ ungrabbed, since they are only grabbed at startup
  
  for (it = node->children.begin(); it != end; ++it) {
    if ( (*it)->action ) {
      for (head_it = _head->children.begin(); head_it != head_end; ++head_it) {
        if ( (*it)->action->modifierMask() == (*head_it)->action->modifierMask() &&
             (*it)->action->keycode() == (*head_it)->action->keycode())
        {
          ungrab = false;
          break;
        }
      }
      
      if (ungrab) 
        scr->ungrabKey( (*it)->action->keycode(), (*it)->action->modifierMask());
      
      ungrab = true;
    }
  }
}

const Action * keytree::getAction(const XKeyEvent * const e, unsigned int & state,
				  ScreenHandler *scr)
{
  Action *act;

  // we're done with the children. ungrab them
  if (_current != _head)
    ungrabChildren(_current, scr);

  // With XKB e->xkey.state can hold the group index in high bits, in
  // addition to the standard modifier bits.  This does not happen on the
  // first grabbed event, but happens when doing stacked cycling (when
  // XGrabKeyboard is active).  In order to recognize subsequent keypresses,
  // we must clear all unneeded bits in the state field.

  state &= (ShiftMask | LockMask | ControlMask | Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask);

  ChildList::const_iterator it, end = _current->children.end();
  for (it = _current->children.begin(); it != end; ++it) {
    act = (*it)->action;

    if (e->keycode == act->keycode() && state == act->modifierMask()) {

      if (act->type() == Action::cancelChain ) {
        // user is cancelling the chain explicitly
        _current = _head;
        return act;
      }
      else if ( isLeaf(*it) ) {
        // node is a leaf, so an action will be executed
        _current = _head;
        return act;
      }
      else {
        // node is not a leaf, so we advance down the tree, and grab the
        // children of the new current node. no action is executed
        _current = *it;
        grabChildren(_current, scr);
        return act;
      }
    }
  }

  // action not found. back to the head
  _current = _head;
  return (const Action *)NULL;
}

void keytree::addAction(Action::ActionType action, unsigned int mask,
                        string key, string arg)
{
  if (action == Action::toggleGrabs && _current != _head) {
    // the toggleGrabs key can only be set up as a root key, since if
    // it was a chain key, we'd have to not ungrab the whole chain up
    // to that key. which kinda defeats the purpose of this function.
    return;
  }

  KeySym sym = XStringToKeysym(key.c_str());
  KeyCode keyCode = XKeysymToKeycode(_display, sym);

  if (keyCode == 0) {
    cerr << BBTOOL << ": " << "keytree::addAction: keyCode for key: [" << key
         << "] not found. can't add it. skipping.\n";
    return;
  }
  
  keynode *tmp = new keynode;

  tmp->action = new Action(action, keyCode, mask, arg);
  tmp->parent = _current;
  _current->children.push_back(tmp);
}

void keytree::advanceOnNewNode()
{
  keynode *tmp = new keynode;
  tmp->action = NULL;
  tmp->parent = _current;
  _current->children.push_back(tmp);
  _current = tmp;
}

void keytree::retract()
{
  if (_current != _head)
    _current = _current->parent;
}

void keytree::setCurrentNodeProps(Action::ActionType action, unsigned int mask,
                                  string key, string arg)
{
  if (_current->action)
    delete _current->action;

  KeySym sym = XStringToKeysym(key.c_str());
  _current->action = new Action(action,
                                XKeysymToKeycode(_display, sym),
                                mask, arg);
}

void keytree::showTree(keynode *node) {
  if (!node)
    return;

  ChildList::iterator it, end = node->children.end();
  for (it = node->children.begin(); it != end; ++it)
    showTree(*it);

  if (node->action) {

    string key;
    KeySym _sym = XKeycodeToKeysym(_display, node->action->keycode(), 0);

    if (_sym == NoSymbol) key="key not found";
    else key = XKeysymToString(_sym);
    
    cout << BBTOOL << ": " << "action: [" << node->action->getActionName()
         << "], key: [" << key
         << "], mask: [" << node->action->modifierMask()
         << "], string: [" << node->action->string() << "]\n";
  }
    
}

