// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- KeyClient.cpp --
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

#ifdef    HAVE_SIGNAL_H
#  include <signal.h>
#endif // HAVE_SIGNAL_H

#ifdef    HAVE_SYS_SIGNAL_H
#  include <sys/signal.h>
#endif // HAVE_SYS_SIGNAL_H

#ifdef	  HAVE_UNISTD_H
# include <unistd.h>
# include <sys/types.h>
#endif // HAVE_UNISTD_H

#ifdef    HAVE_SYS_STAT_H
#  include <sys/types.h>
#  include <sys/stat.h>
#endif // HAVE_SYS_STAT_H

#include <sys/types.h>
#include <sys/wait.h>

}

#include "version.h"

#include "KeyClient.h"
#include "LocalUtil.h"
#include "actions.hh"

#include <iostream>
#include <algorithm>
#include <vector>

//--------------------------------------------------------
// Constructor/Destructor
//--------------------------------------------------------
KeyClient::KeyClient (int argc, char **argv,
                      Config & config, std::string display):
  bt::Application(BBTOOL, display.c_str(), true), _config(config)
{

  // save off what we're constructed with for reconfiguring later...
  _argc = argc;
  _argv = argv;

  // initialize our keyword map for the file tokenizer
  initKeywords(_keywordMap);

  // now connect to the X server
  _display = XDisplay();
  if (! _display ) {
    cerr << "KeyClient: ERROR: Can't connect to X Server. Bummer! Exiting\n";
    exit(2);
  }

  // check to see if we've been handed another config-file to use
  _configFileName = bt::expandTilde(_config.getStringValue("config",
                                    "~/.bbkeysrc") );

  struct stat buf;
  if (0 != stat(_configFileName.c_str(), &buf) ||!S_ISREG(buf.st_mode)) {
    cerr << "KeyClient: ERROR: Couldn't load rc-file: [" << _configFileName
         << "], falling back to default: [" << DEFAULTRC << "]\n";
    _configFileName = DEFAULTRC;
  } else {
    _last_time_config_changed = buf.st_mtime;
  }

  _debug = _config.getBoolValue("debug", false);

  // here's our friendly little general-purpose keygrabber
  _keyGrabber = new KeyGrabber(_display, numLockMask(), scrollLockMask() );

  _netclient = new Netclient(_display);
  _active = _clients.end();

  initialize();
}

KeyClient::~KeyClient ()
{

  // delete all screens
  for_each(screenList.begin(), screenList.end(), bt::PointerAssassin());

  if (_keybindings) delete _keybindings;
  if (_netclient) delete _netclient;
  if (_keyGrabber) delete _keyGrabber;
}

void KeyClient::initialize() {

  // now, read in our configuration file and set both program settings
  // and keybindings we're asked to handle
  handleConfigFile();

  // parse command options again to override what we read in from config file
  parseOptions( _argc, _argv, _config );


  // now create a screen handler for each screen that exists
  for (unsigned int i = 0; i < bt::Application::display().screenCount(); i++) {
    ScreenHandler *screen = new ScreenHandler(this, i);
    if (! screen->isManaged()) {
      delete screen;
      continue;
    }

    screen->initialize();

    // add this screen to our collection
    screenList.push_back(screen);
  }

  if (screenList.empty()) {
    cerr << "KeyClient: initialize: no compatible window managers found, aborting.\n";
    ::exit(3);
  }

  _autoConfigCheckTimeout = (_config.getNumberValue("autoConfigCheckTimeout", 10)) * 1000;
  _autoConfig = _config.getBoolValue("autoConfig", true);
  if (_autoConfig) {
    if (!config_check_timer) {
      config_check_timer = new bt::Timer(this, this);
    }
    config_check_timer->setTimeout(_autoConfigCheckTimeout);
    config_check_timer->recurring(True);
    config_check_timer->start();
  }

}

//--------------------------------------------------------
// reconfigure
//--------------------------------------------------------
void KeyClient::reconfigure ()
{
  if (_debug)
    std::cout << "KeyClient: reconfigure: hey, goodie! I got a reconfigure request!!\n";


  // delete all screens
  for_each(screenList.begin(), screenList.end(), bt::PointerAssassin());
  screenList.clear();

  // initialize and/or clear our config
  _config.reset();

  // reset our timer
  if (config_check_timer) {
    config_check_timer->halt();
  }

  initialize();

}


void KeyClient::handleConfigFile() {

  FileTokenizer tokenizer(_keywordMap, _configFileName.c_str());

  // clear off any of our keybindings we have and get them ready to go
  if (_keybindings) {
    _keybindings->unloadBindings();
  } else {
    _keybindings = new keytree(_display);
  }

  _keybindings->reset();

  bool _doingConfig = false, _doingKeybindings = false;

  TokenBlock *block = 0;
  while ((block = tokenizer.next())) {
    switch (block->tag) {
    case ConfigOpts::begin: // um, we ignore these. =:)
      break;
    case ConfigOpts::end:
      if (_doingConfig) {
        _doingConfig = false;
      }
      break;
    case ConfigOpts::config:
      _doingConfig = true;
      break;
    case ConfigOpts::option:
      if (_debug)
        cout << "got a config option!, setting key: [" << block->name
             << "] to value: [" << block->data << "]\n";

      _config.setOption(block->name, block->data);
      break;
    case ConfigOpts::keybindings:
      _doingKeybindings = true;
      setKeybindings(tokenizer);

      if (_debug)
        _keybindings->showTree();

      break;
    default:
      cerr << "unknown tag found in ConfigOpts block: ["
           << block->tag << "], name: [" << block->name
           << "], data: [" << block->data << "]\n";
      break;
    }
    delete block;
  }

  if (_debug) {
    cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    _config.showOptions();
  }

}

void KeyClient::setKeybindings(FileTokenizer & tokenizer) {

  // our modifier masks
  struct {
    const char *str;
    unsigned int mask;
  }
  modifiers[] = {
    { "mod1", Mod1Mask },
    { "mod2", Mod2Mask },
    { "mod3", Mod3Mask },
    { "mod4", Mod4Mask },
    { "mod5", Mod5Mask },
    { "control", ControlMask },
    { "shift", ShiftMask },
    { "", 0 }
  };

  // this tells us how many levels deep we're nested. this will tell us when to return
  int _iLevels =0;

  TokenBlock *block = 0;
  while ((block = tokenizer.next())) {

    // if we hit an end, return
    if (block->tag == ConfigOpts::end) {
      --_iLevels;
      _keybindings->retract();
      // 0 is our root level, so if we're below that, we need to bail out
      if (_iLevels <0) {
        if (block) delete block;
        return;
      }
    } else {

      string fullKey = block->name;

      if (fullKey.size() <=0) {
        cerr << "ERROR: No key or modifier given. Ignoring this one, Jimmy.\n";
        if (block) delete block;
        continue;
      }
      // first, split our string containing our keys/modifiers and separate
      // the keys from the modifiers
      vector<string> results;
      int matches = LocalUtil::splitString(fullKey, "-", results);

      // here's our keyname. make sure it's a valid key
      string _key = results[results.size() -1];

      KeySym sym = XStringToKeysym(_key.c_str());
      if (sym == 0) {
        cerr << "ERROR: Invalid key (" << _key << ")! This may cause odd behavior.\n";
      }

      // now iterate through our modifiers and try to match the given string
      // to a modifier mask. if we find it, xor it together with what we already have
      unsigned int _mask=0;

      for (int j=0; j < (matches -1); j++) {

        bool found=false;
        string mod = results[j];

        for (int i = 0; modifiers[i].str != ""; ++i) {
          if ( strcasecmp(modifiers[i].str, mod.c_str()) == 0 ) {
            _mask |= modifiers[i].mask;
            found = true;
            break;
          }
        }
        if (!found) cerr << "ERROR: Couldn't find modifier for mod: [" << mod << "]\n";
      }

      // now, if we have a chain, nest down a level and add the keybinding
      if (block->tag == Action::chain) {
        _iLevels++;
        _keybindings->advanceOnNewNode();
        _keybindings->setCurrentNodeProps(static_cast<Action::ActionType>(block->tag),
                                          _mask, _key, block->data);
      } else {
        _keybindings->addAction(static_cast<Action::ActionType>(block->tag),
                                _mask, _key, block->data);
      }
    }
    if (block) delete block;
  }
}

void KeyClient::initKeywords(KeywordMap& keywords) {

  // load our map with our keybinding labels
  keywords.insert(KeywordMap::value_type("execute", Action::execute));
  keywords.insert(KeywordMap::value_type("iconify", Action::iconify));
  keywords.insert(KeywordMap::value_type("raise", Action::raise));
  keywords.insert(KeywordMap::value_type("lower", Action::lower));
  keywords.insert(KeywordMap::value_type("close", Action::close));
  keywords.insert(KeywordMap::value_type("toggleshade", Action::toggleShade));
  keywords.insert(KeywordMap::value_type("toggleomnipresent", Action::toggleOmnipresent));
  keywords.insert(KeywordMap::value_type("movewindowup", Action::moveWindowUp));
  keywords.insert(KeywordMap::value_type("movewindowdown", Action::moveWindowDown));
  keywords.insert(KeywordMap::value_type("movewindowleft", Action::moveWindowLeft));
  keywords.insert(KeywordMap::value_type("movewindowright", Action::moveWindowRight));
  keywords.insert(KeywordMap::value_type("resizewindowwidth", Action::resizeWindowWidth));
  keywords.insert(KeywordMap::value_type("resizewindowheight", Action::resizeWindowHeight));

  keywords.insert(KeywordMap::value_type("togglemaximizefull", Action::toggleMaximizeFull));
  keywords.insert(KeywordMap::value_type("togglemaximizevertical", Action::toggleMaximizeVertical));
  keywords.insert(KeywordMap::value_type("togglemaximizehorizontal", Action::toggleMaximizeHorizontal));

  keywords.insert(KeywordMap::value_type("sendtoworkspace", Action::sendToWorkspace));

  keywords.insert(KeywordMap::value_type("nextwindow", Action::nextWindow));
  keywords.insert(KeywordMap::value_type("prevwindow", Action::prevWindow));
  keywords.insert(KeywordMap::value_type("nextwindowonallworkspaces", Action::nextWindowOnAllWorkspaces));
  keywords.insert(KeywordMap::value_type("prevwindowonallworkspaces", Action::prevWindowOnAllWorkspaces));

  keywords.insert(KeywordMap::value_type("changeworkspace", Action::changeWorkspace));
  keywords.insert(KeywordMap::value_type("nextworkspace", Action::nextWorkspace));
  keywords.insert(KeywordMap::value_type("prevworkspace", Action::prevWorkspace));

  keywords.insert(KeywordMap::value_type("upworkspace", Action::upWorkspace));
  keywords.insert(KeywordMap::value_type("downworkspace", Action::downWorkspace));
  keywords.insert(KeywordMap::value_type("leftworkspace", Action::leftWorkspace));
  keywords.insert(KeywordMap::value_type("rightworkspace", Action::rightWorkspace));

  keywords.insert(KeywordMap::value_type("nextscreen", Action::nextScreen));
  keywords.insert(KeywordMap::value_type("prevscreen", Action::prevScreen));

  keywords.insert(KeywordMap::value_type("showrootmenu", Action::showRootMenu));
  keywords.insert(KeywordMap::value_type("showworkspacemenu", Action::showWorkspaceMenu));
  keywords.insert(KeywordMap::value_type("toggledecorations", Action::toggleDecorations));

  keywords.insert(KeywordMap::value_type("togglegrabs", Action::toggleGrabs));
  keywords.insert(KeywordMap::value_type("chain", Action::chain));

  // the words associated with our high-level file labels
  keywords.insert(KeywordMap::value_type("begin", ConfigOpts::begin));
  keywords.insert(KeywordMap::value_type("end", ConfigOpts::end));
  keywords.insert(KeywordMap::value_type("config", ConfigOpts::config));
  keywords.insert(KeywordMap::value_type("keybindings", ConfigOpts::keybindings));
  keywords.insert(KeywordMap::value_type("option", ConfigOpts::option));

}

bool KeyClient::process_signal(int sig) {
  switch (sig) {
  case SIGHUP:
    reconfigure();
    return true;
    break;

  default:
    return (bt::Application::process_signal(sig) );
  }

}

void KeyClient::process_event(XEvent *e) {
  // Send the event through the default EventHandlers.
  bt::Application::process_event(e);
}

void KeyClient::cycleScreen(int current, bool forward) const {
  unsigned int i;
  for (i = 0; i < screenList.size(); ++i)
    if (screenList[i]->screenNumber() == current) {
      current = i;
      break;
    }
  assert(i < screenList.size());  // current is for an unmanaged screen

  int dest = current + (forward ? 1 : -1);

  if (dest < 0) dest = (signed)screenList.size() - 1;
  else if (dest >= (signed)screenList.size()) dest = 0;

  const XWindow *target = screenList[dest]->lastActiveWindow();
  if (target) target->focus();
}

void KeyClient::timeout(bt::Timer *timer) {
  if (_debug)
    std::cout << "KeyClient: timeout: got timeout from timer...." << std::endl;
  if (timer == config_check_timer) {
    checkConfigFile();
  }
}

void KeyClient::checkConfigFile() {

  struct stat file_status;

  if (stat(_configFileName.c_str(), &file_status) != 0) {
    if (_debug)
      std::cerr << "Could not open config file: [" << _configFileName << "]";
  } else if (file_status.st_mtime != _last_time_config_changed) {
    if (_debug)
      std::cout << "KeyClient: checkConfigFile: config file time changed..." << std::endl;
    _last_time_config_changed = file_status.st_mtime;
    reconfigure();
  }
}

