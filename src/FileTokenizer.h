// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- FileTokenizer.h --
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

#ifndef FILETOKENIZER_HH
#define FILETOKENIZER_HH

#include <string>
#include <iostream>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <map>
#include "version.h"

using namespace std;

typedef map<string,unsigned int> KeywordMap;

static bool defaultTokenError(const string& filename, const string& msg,
                              unsigned long lineno) {
    cerr << filename << ":" << lineno << " " << msg << '\n';
    return true;
}

struct TokenBlock {
    unsigned int tag;
    string name, data;
    unsigned long lineno;
};

class FileTokenizer {
 public:
    typedef bool (*TokenError)(const string&, const string&, unsigned long);
    FileTokenizer(const KeywordMap& keys, const char* filename,
                  TokenError errhandler = defaultTokenError);
    ~FileTokenizer(void);
    TokenBlock* next(void);

 private:
    ifstream file;
    string filename;
    const KeywordMap& keywords;
    TokenError error;
    unsigned long lineno;
    enum parse_state { WANT_NOTHING, WANT_TAG, WANT_NAME, WANT_DATA };
    parse_state state;
};

#endif
