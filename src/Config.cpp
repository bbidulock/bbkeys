// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- Config.cpp --
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


#include "Config.h"
#include <string>

#include <iostream>
using std::cout;

Config::Config() {
}

Config::~Config()
{
  _configMap.clear();
}

bool Config::getBoolValue(const std::string & key, const  bool & iDefault)
{
  std::string _key=key;
  std::transform(_key.begin(), _key.end(), _key.begin(), tolower);

  const ConfigMap::const_iterator it= _configMap.find(_key);
  if (it != _configMap.end()) {
    std::string value = it->second;

    if (strcasecmp(value.c_str(), "true") == 0 || strcasecmp(value.c_str(), "1") == 0 ||
        strcasecmp(value.c_str(), "on") == 0)
      return true;
    else
      return false;

  } else {
    return iDefault;
  }

}

int Config::getNumberValue(const std::string & key, const int & iDefault)
{
  std::string _key=key;
  std::transform(_key.begin(), _key.end(), _key.begin(), tolower);

  const ConfigMap::const_iterator it= _configMap.find(_key);
  if (it != _configMap.end()) {
    std::string value = it->second;

    return (atoi(value.c_str()));

  } else {
    return iDefault;
  }

}

std::string Config::getStringValue(const std::string & key, const std::string & iDefault)
{
  std::string _key=key;
  std::transform(_key.begin(), _key.end(), _key.begin(), tolower);

  const ConfigMap::const_iterator it= _configMap.find(_key);
  if (it != _configMap.end()) {
    std::string value = it->second;

    return value;

  } else {
    return iDefault;
  }

}


void Config::setOption(const std::string &name, const std::string &value)
{
  std::string key=name;
  std::transform(key.begin(), key.end(), key.begin(), tolower);

  const ConfigMap::const_iterator it= _configMap.find(key);
  if (it != _configMap.end())
    _configMap.erase(ConfigMap::key_type(key));

  _configMap.insert(ConfigMap::value_type(key, value));

}

void Config::showOptions() {
  ConfigMap::const_iterator it = _configMap.begin(), end = _configMap.end();
  for (; it != end; ++it) {
    cout << "key: [" << it->first << "], value: [" << it->second << "]\n";
  }

}
