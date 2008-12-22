// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- main.cpp --
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

#include "KeyClient.h"
#include "version.h"
#include <string.h>

#include "main.h"

//--------------------------------------------------------
// parseOptions
//--------------------------------------------------------
void parseOptions (int argc, char **argv, Config & _config)
{

  for (int i = 1; i < argc; i++) {
    if ( (!strcmp(argv[i], "-d"))
         || (!strcmp(argv[i], "--display")) ) {
      if (++i == argc) {
        usage();
        exit(2);
      };
      _config.setOption("display", argv[i]);

      // Applications we exec will need the proper display
      string disp = (string)"DISPLAY=" + argv[i];
      putenv((char *)disp.c_str());

    } else if ((!strcmp(argv[i], "--config")) ||
               (!strcmp(argv[i], "-c"))) {
      if (++i == argc) {
        usage();
        exit(2);
      };
      _config.setOption("config", argv[i]);

    } else if ((!strcmp(argv[i], "-D"))
               || (!strcmp(argv[i], "--debug"))) {
      _config.setOption("debug", "true");
    } else if ((!strcmp(argv[i], "-v"))
               || (!strcmp(argv[i], "--version"))) {
      cout << BBTOOL << " version: [" <<  BBTOOL_VERSION << "]" << endl;
      exit(2);
    } else if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "-help"))) {
      usage();
      exit(2);

    } else {
      usage();
      exit(2);
    };

  }
}

void usage() {

  cout <<  BBTOOL <<  " version: [" << BBTOOL_VERSION << "]" << endl
       << "Usage: " << BBTOOL << " [options]" << endl
       << "Options:" << endl
       << " -d or --display <display name>    X server to connect to" << endl
       << " -D or --debug                     print debugging information" << endl
       << " -c or --config  <filename>        configuration file" << endl
       << "                     (default is ~/.bbkeysrc)" << endl
       << " -v or --version                   Display version number" << endl
       << " -h or --help                      Display this help" << endl;
}



int main(int argc, char **argv)
{
  Config * _config = new Config();
  parseOptions(argc, argv, *_config);

  std::string dpy_name = _config->getStringValue("display",
                                                 getenv("DISPLAY"));


  KeyClient * _k=new KeyClient(argc, argv, *_config, dpy_name);
  _k->run();

  delete _k;
  delete _config;

  return 0;
}
