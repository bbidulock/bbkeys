// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- Config.h --
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

#ifndef __config_hh
#define __config_hh

#include <string>
#include <list>
#include <map>
#include <algorithm>
#include "version.h"

typedef std::map<std::string, std::string> ConfigMap;

class Config {
private:
  ConfigMap _configMap;

public:
  Config();
  ~Config();

  bool getBoolValue(const std::string & key, const bool & defaultValue);
  int getNumberValue(const std::string & key, const int & defaultValue);
  std::string getStringValue(const std::string & key, const std::string & defaultValue);

  void setOption(const std::string & key, const std::string & value);
  void showOptions();
  void reset();
};


#endif // __config_hh
