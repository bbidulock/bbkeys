// -*- mode: C++; indent-tabs-mode: nil; -*-
// actions.cc for Epistrophy - a key handler for NETWM/EWMH window managers.
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

#include "actions.hh"

#include <iostream>
using std::cout;

Action::Action(enum ActionType type, KeyCode keycode, unsigned int modifierMask,
               const std::string &str)
  : _type(type), _keycode(keycode), _modifierMask(modifierMask)
{
  // These are the action types that take string arguments. This
  // should probably be moved to a static member
  ActionType str_types[] = {
    execute,
    nextWindowOfClass,
    prevWindowOfClass,
    nextWindowOfClassOnAllWorkspaces,
    prevWindowOfClassOnAllWorkspaces,
    noaction
  };

  for (int i = 0; str_types[i] != noaction; ++i) {
    if (type == str_types[i]) {
      _stringParam = str;
      return;
    }
  }
 
  _numberParam = atoi( str.c_str() );

  // workspace 1 to the user is workspace 0 to us
  if (type == changeWorkspace || type == sendToWorkspace)
    _numberParam--;
}

const char * Action::getActionName() {
  struct {
    const char* str;
    Action::ActionType act;
  }
  actions[] = {
    { "noaction", Action::noaction },
    { "execute", Action::execute },
    { "iconify", Action::iconify },
    { "raise", Action::raise },
    { "lower", Action::lower },
    { "close", Action::close },
    { "toggleShade", Action::toggleShade },
    { "toggleOmnipresent", Action::toggleOmnipresent },
    { "movewindowup", Action::moveWindowUp },
    { "movewindowdown", Action::moveWindowDown },
    { "movewindowleft", Action::moveWindowLeft },
    { "movewindowright", Action::moveWindowRight },
    { "resizewindowwidth", Action::resizeWindowWidth },
    { "resizewindowheight", Action::resizeWindowHeight },
    { "togglemaximizefull", Action::toggleMaximizeFull },
    { "togglemaximizevertical", Action::toggleMaximizeVertical },
    { "togglemaximizehorizontal", Action::toggleMaximizeHorizontal },
    { "sendtoworkspace", Action::sendToWorkspace },
    { "nextwindow", Action::nextWindow },
    { "prevwindow", Action::prevWindow },
    { "nextwindowonallworkspaces", Action::nextWindowOnAllWorkspaces },
    { "prevwindowonallworkspaces", Action::prevWindowOnAllWorkspaces },
    { "nextwindowonallscreens", Action::nextWindowOnAllScreens },
    { "prevwindowonallscreens", Action::prevWindowOnAllScreens },
    { "nextwindowofclass", Action::nextWindowOfClass },
    { "prevwindowofclass", Action::prevWindowOfClass },
    { "nextwindowofclassonallworkspaces", Action::nextWindowOfClassOnAllWorkspaces },
    { "prevwindowofclassonallworkspaces", Action::prevWindowOfClassOnAllWorkspaces },
    { "changeworkspace", Action::changeWorkspace },
    { "nextworkspace", Action::nextWorkspace },
    { "prevworkspace", Action::prevWorkspace },
    { "nextworkspacerow", Action::upWorkspace },
    { "prevworkspacerow", Action::downWorkspace },
    { "prevworkspacecolumn", Action::leftWorkspace },
    { "nextworkspacecolumn", Action::rightWorkspace },
    { "nextscreen", Action::nextScreen },
    { "prevscreen", Action::prevScreen },
    { "showrootmenu", Action::showRootMenu },
    { "showworkspacemenu", Action::showWorkspaceMenu },
    { "toggledecorations", Action::toggleDecorations },
    { "togglegrabs", Action::toggleGrabs },
    { "stringchain", Action::stringChain },
    { "keychain", Action::keyChain },
    { "numberchain", Action::numberChain },
    { "cancelchain", Action::cancelChain },
    { "chain", Action::chain },
    { "", Action::noaction }
  };

  for (int i = 0; actions[i].str != ""; ++i) {
    if ( actions[i].act ==  _type) {
        return actions[i].str;
    }
  }

  cout << "ERROR: Invalid action (" << _type << ").\n";
  return "not found";
  
}

