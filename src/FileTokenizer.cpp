// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- FileTokenizer.cpp --
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

#include "FileTokenizer.h"

static unsigned int keywordLookup(const KeywordMap& keys, const string& tag) {
    const KeywordMap::const_iterator it = keys.find(tag);
    if (it != keys.end())
        return it->second;
    return 0;
}


static char tabToSpace(char c) {
    if (c == '\t')
        return ' ';
    return c;
}


/* read a line into the string 'line', if it is continued then keep reading
 * until we have the whole line
 * returns the number of lines read and the line via the reference
 */
static unsigned int getFullLine(ifstream& file, string& line) {
    if (! file.good()) return 0;

    getline(file, line);
    unsigned int count = 1;
    while (file.good() && line[line.length() - 1] == '\\') {
        string tmp;
        getline(file, tmp);
        line.erase(line.length() - 1);
        line += tmp;
        ++count;
    }
    transform(line.begin(), line.end(), line.begin(), tabToSpace);
    return count;
}



FileTokenizer::FileTokenizer(const KeywordMap& keys, const char* fname,
			     TokenError errhandler): filename(fname),
						     keywords(keys),
						     error(errhandler),
						     lineno(0l),
						     state(WANT_TAG) {
    file.open(filename.c_str());

    if (! file.good()) {
        cerr << BBTOOL << ": " << "FileTokenizer::constructor: couldn't open file: [" << filename << "]" << endl;
    }
}


FileTokenizer::~FileTokenizer(void) {
    file.close();
}


TokenBlock* FileTokenizer::next(void) {
    string line;

 TOP:
    unsigned int count;
    if ((count = getFullLine(file, line)) == 0)
        return (TokenBlock*) 0;
    lineno += count;

    TokenBlock* block = new TokenBlock;
    block->lineno = lineno;

    string::size_type pos = 0, len = line.length();
    for (; state != WANT_NOTHING && pos < len; ++pos) {
        if (isspace(line[pos])) continue;

        if (state == WANT_TAG && line[pos] == '[') {
            string::size_type start = ++pos;
            while (pos < len && line[pos] != ']') ++pos; // no escapes allowed

            string tag(line, start, pos - start);
            transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
            unsigned int value = keywordLookup(keywords, tag);
            if (value == 0) {
                if (! error(filename, "unknown tag: " + tag, lineno))
                    goto ERR_EXIT;
                goto TOP;
            }
            block->tag = value;
            state = WANT_NAME;
        } else if (state == WANT_NAME && line[pos] == '(') {
            string::size_type start = ++pos;
            while (pos < len && line[pos] != ')') {
                if (line[pos++] == '\\') ++pos;
            }

            block->name.assign(line, start, pos - start);
            state = WANT_DATA;
        } else if (state == WANT_DATA && line[pos] == '{') {
            string::size_type start = ++pos;
	    // now start to search for the closing curly bracket at the end
	    // as there might be a curly bracket in between starting and
	    // ending brackets
            pos = len;
            while (pos > start && line[pos] != '}') {
		    --pos;
            }

            block->data.assign(line, start, pos - start);
            state = WANT_NOTHING;
        } else {
            if (line[pos] != '#') { // this isn't a comment, complain
                const string e = "invalid input: " + string(line, pos, line.length());
                if (! error(filename, e, lineno))
                    goto ERR_EXIT;
            }
            break;
        }
    }
    if (state == WANT_TAG) {
        // we never found what we wanted, so let's get a new line and try again
        delete block;
        goto TOP;
    }

    state = WANT_TAG; // reset state for next call

    return block;

 ERR_EXIT:
    delete block;
    return (TokenBlock*) 0;
}

