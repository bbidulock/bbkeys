// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- LocalUtil.h --
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

#ifndef LOCALUTIL_HH
#define LOCALUTIL_HH

#include <string>
#include <fstream>
#include <vector>
#include <iostream>

class LocalUtil
{

public:

// trim_right() family.
  static inline std::string trim_right ( const std::string & source ,
                                         const std::string & t = " " )
    {
      std::string str = source;
      return str.erase ( str.find_last_not_of ( t ) + 1 ) ;
    }
 
// trim_left() family.
  static inline std::string trim_left ( const std::string & source ,
                                        const std::string & t = " " )
    {
      std::string str = source;
      return str.erase ( 0 , source.find_first_not_of ( t ) ) ;
    }
 
// trim() family.
  static inline std::string trim ( const std::string & source ,
                                   const std::string & t = " " )
    {
      std::string str = source;
      return trim_left ( trim_right ( str , t ) , t ) ;
    }

  static unsigned int splitString(const std::string & _input, const std::string & _delim,
                           std::vector<string>& results) {
    const std::string::size_type sizeS1 = _input.size(),sizeS2 = _delim.size();
    std::string::size_type iPos = 0, newPos = iPos;

    do {
      newPos = _input.find(_delim, iPos);
      if (newPos == iPos) {
        iPos += sizeS2;
        continue;
      }
      if (newPos == std::string::npos) {
        results.push_back(_input.substr(iPos, sizeS1 - iPos));
        iPos = sizeS1; // iPos points to end of string
      } else {
        results.push_back(_input.substr(iPos, newPos - iPos));
        iPos = newPos + sizeS2;
      }
    } while (iPos < sizeS1);

    return results.size();
  }

};
#endif
