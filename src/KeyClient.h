// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- KeyClient.h --
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

#ifndef  KEYCLIENT_HH
#define  KEYCLIENT_HH

#include "FileTokenizer.h"
#include "ConfigOptions.h"
#include "actions.hh"
#include "keytree.hh"
#include "window.hh"
#include "Config.h"
#include "ScreenHandler.h"
#include "KeyGrabber.h"
#include "Netclient.h"
#include "main.h"

#include <string>
#include <stdio.h>

// the blackbox library
#include <Application.hh>
#include <Util.hh>
#include <Resource.hh>
#include <Timer.hh>


class ScreenHandler;
class keytree;

class KeyClient : public bt::Application, public bt::TimeoutHandler
{

public:
  KeyClient (int argc, char **argv,
             Config & config, std::string display);
  ~KeyClient () ;
  keytree * getKeybindings () {return _keybindings;}
  inline Netclient * getNetclient() { return _netclient; }
  inline Config * getConfig() { return &_config; }
  inline KeyGrabber * getKeyGrabber() { return _keyGrabber; }
  inline bt::Application & getMainApplication()
    { return dynamic_cast<bt::Application&> (* this); }
  void cycleScreen(int current, bool forward) const;

  void getLockModifiers(int &numlockMask, int &scrolllockMask) const {
    numlockMask = numLockMask();
    scrolllockMask = scrollLockMask();
  }

  WindowList& clientsList() { return _clients; }
  WindowList::iterator& activeWindow() { return _active; }
  virtual void timeout(bt::Timer *timer);

private:
  int _argc;
  char **_argv;
  keytree *_keybindings;
  std::string _configFileName;
  Config & _config;
  KeywordMap _keywordMap;
  bt::Netwm * _netwm;
  Netclient * _netclient;
  Display * _display;
  KeyGrabber * _keyGrabber;
  bool _debug;

  typedef std::vector<ScreenHandler*> ScreenList;
  ScreenList screenList;

  void handleConfigFile();

  void initKeywords(KeywordMap &);
  void reconfigure();
  void initialize();
  void setKeybindings(FileTokenizer &);

  bool process_signal(int sig);
  void process_event(XEvent *e);

  WindowList _clients;
  WindowList::iterator _active;

  bt::Timer *config_check_timer;
  long _autoConfigCheckTimeout;
  bool _autoConfig;
  void checkConfigFile();
  time_t _last_time_config_changed;

};
#endif
