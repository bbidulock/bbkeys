// -*- mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; -*-
// -- Netclient.cpp --
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

// Methods, ideas, implementations taken from Openbox's XAtom class *sigh*

#include "Netclient.h"

Netclient::Netclient (Display * display) : bt::Netwm(display)
{

  _display = display;
  init_icccm();
  init_extras();
  init_blackbox();
    
}
                      
Netclient::~Netclient ()
{
}

void Netclient::init_icccm(void) {
  char* atoms[9] = {
    "WM_COLORMAP_WINDOWS",
    "WM_PROTOCOLS",
    "WM_NAME",
    "WM_STATE",
    "WM_CLASS",
    "WM_CHANGE_STATE",
    "WM_DELETE_WINDOW",
    "WM_TAKE_FOCUS",
    "_MOTIF_WM_HINTS"
  };
  Atom atoms_return[9];
  XInternAtoms(_display, atoms, 9, False, atoms_return);
  xa_wm_colormap_windows = atoms_return[0];
  xa_wm_protocols = atoms_return[1];
  xa_wm_name = atoms_return[2];
  xa_wm_state = atoms_return[3];
  xa_wm_class = atoms_return[4];
  xa_wm_change_state = atoms_return[5];
  xa_wm_delete_window = atoms_return[6];
  xa_wm_take_focus = atoms_return[7];
  motif_wm_hints = atoms_return[8];

}

void Netclient::init_extras(void) {
  char* atoms[4] = {
    "_OPENBOX_SHOW_ROOT_MENU",
    "_OPENBOX_SHOW_WORKSPACE_MENU",
    "ENLIGHTENMENT_DESKTOP",
    "_NET_VIRTUAL_ROOTS"
  };
  Atom atoms_return[4];
  XInternAtoms(_display, atoms, 2, False, atoms_return);
  openbox_show_root_menu = atoms_return[0];
  openbox_show_workspace_menu = atoms_return[1];
  enlightenment_desktop = atoms_return[2];
  net_virtual_roots = atoms_return[3];

}

void Netclient::init_blackbox(void) {
  char* atoms[3] = {
    "_BLACKBOX_HINTS",
    "_BLACKBOX_ATTRIBUTES",
    "_BLACKBOX_CHANGE_ATTRIBUTES"
  };
  Atom atoms_return[3];
  XInternAtoms(_display, atoms, 3, False, atoms_return);
  blackbox_hints = atoms_return[0];
  blackbox_attributes = atoms_return[1];
  blackbox_change_attributes = atoms_return[2];

}

std::string Netclient::getWindowTitle(Window win) const
{

  std::string _title = "";

  // try netwm
  if (! getValue(win, wmName(), utf8, _title)) {
    // try old x stuff
    getValue(win, XA_WM_NAME, ansi, _title);
  }

  if (_title.empty())
    _title = "Unnamed";

  return _title;
}

unsigned int Netclient::getDesktop(Window win) const {
  unsigned long _desktop = 0ul;
  
  if (! getValue(win, wmDesktop(), XA_CARDINAL, _desktop))
    _desktop = 0ul;

  return static_cast<unsigned int>(_desktop);
}

/*
 * Internal getValue function used by all of the typed getValue functions.
 * Gets an property's value from a window.
 * Returns True if the property was successfully retrieved; False if the
 * property did not exist on the window, or has a different type/size format
 * than the user tried to retrieve.
 */
bool Netclient::getValue(Window win, Atom atom, Atom type,
                         unsigned long &nelements, unsigned char **value,
                         int size) const {
  assert(win != None); assert(atom != None); assert(type != None);
  assert(size == 8 || size == 16 || size == 32);
  assert(nelements > 0);
  unsigned char *c_val = 0;        // value alloc'd in Xlib, must be XFree()d
  Atom ret_type;
  int ret_size;
  unsigned long ret_bytes;
  int result;
  unsigned long maxread = nelements;
  bool ret = False;

  // try get the first element
  result = XGetWindowProperty(_display, win, atom, 0l, 1l, False,
                              AnyPropertyType, &ret_type, &ret_size,
                              &nelements, &ret_bytes, &c_val);
  ret = (result == Success && ret_type == type && ret_size == size &&
         nelements > 0);
  if (ret) {
    if (ret_bytes == 0 || maxread <= nelements) {
      // we got the whole property's value
      *value = new unsigned char[nelements * size/8 + 1];
      memcpy(*value, c_val, nelements * size/8 + 1);
    } else {
      // get the entire property since it is larger than one long
      XFree(c_val);
      // the number of longs that need to be retreived to get the property's
      // entire value. The last + 1 is the first long that we retrieved above.
      int remain = (ret_bytes - 1)/sizeof(long) + 1 + 1;
      if (remain > size/8 * (signed)maxread) // dont get more than the max
        remain = size/8 * (signed)maxread;
      result = XGetWindowProperty(_display, win, atom, 0l, remain, False, type,
                                  &ret_type, &ret_size, &nelements, &ret_bytes,
                                  &c_val);
      ret = (result == Success && ret_type == type && ret_size == size &&
             ret_bytes == 0);
      /*
        If the property has changed type/size, or has grown since our first
        read of it, then stop here and try again. If it shrank, then this will
        still work.
      */
      if (! ret)
        return getValue(win, atom, type, maxread, value, size);
  
      *value = new unsigned char[nelements * size/8 + 1];
      memcpy(*value, c_val, nelements * size/8 + 1);
    }    
  }
  if (c_val) XFree(c_val);
  return ret;
}


/*
 * Gets a 32-bit property's value from a window.
 */
bool Netclient::getValue(Window win, Atom atom, Atom type,
                         unsigned long &nelements,
                         unsigned long **value) const {
  return getValue(win, atom, type, nelements,
                  reinterpret_cast<unsigned char **>(value), 32);
}


/*
 * Gets a single 32-bit property's value from a window.
 */
bool Netclient::getValue(Window win, Atom atom, Atom type,
                         unsigned long &value) const {
  unsigned long *temp;
  unsigned long num = 1;
  if (! getValue(win, atom, type, num,
                 reinterpret_cast<unsigned char **>(&temp), 32))
    return False;

  value = temp[0];
  delete [] temp;
  return True;
}


/*
 * Gets an string property's value from a window.
 */
bool Netclient::getValue(Window win, Atom atom, StringType type,
                         std::string &value) const {
  unsigned long n = 1;
  StringVect s;
  if (getValue(win, atom, type, n, s)) {
    value = s[0];
    return True;
  }
  return False;
}


bool Netclient::getValue(Window win, Atom atom, StringType type,
                         unsigned long &nelements, StringVect &strings) const {
  assert(win != None); assert(atom != None);
  assert(nelements > 0);

  Atom t;
  switch (type) {
  case ansi: t = XA_STRING; break;
  case utf8: t = utf8String(); break;
  default: assert(False); return False; // unhandled StringType
  }

  unsigned char *value;
  unsigned long elements = (unsigned) -1;
  if (!getValue(win, atom, t, elements, &value, 8) || elements < 1)
    return False;

  std::string s(reinterpret_cast<char *>(value), elements);
  delete [] value;

  std::string::const_iterator it = s.begin(), end = s.end();
  unsigned long num = 0;
  while(num < nelements) {
    std::string::const_iterator tmp = it; // current string.begin()
    it = std::find(tmp, end, '\0');       // look for null between tmp and end
    strings.push_back(std::string(tmp, it));   // s[tmp:it)
    ++num;
    if (it == end) break;
    ++it;
    if (it == end) break;
  }

  nelements = num;

  return True;
}


/*
 * Removes a property entirely from a window.
 */
void Netclient::eraseValue(Window win, Atom atom) const {
  XDeleteProperty(_display, win, atom);
}


void Netclient::sendClientMessage(Window target, Atom type, Window about,
                                  long data, long data1, long data2,
                                  long data3, long data4) const {
  assert(target != None);

  XEvent e;
  e.xclient.type = ClientMessage;
  e.xclient.format = 32;
  e.xclient.message_type = type;
  e.xclient.window = about;
  e.xclient.data.l[0] = data;
  e.xclient.data.l[1] = data1;
  e.xclient.data.l[2] = data2;
  e.xclient.data.l[3] = data3;
  e.xclient.data.l[4] = data4;

  XSendEvent(_display, target, False,
             SubstructureRedirectMask | SubstructureNotifyMask,
             &e);
}

bool Netclient::isAtomSupported(Window win, Atom atom) const {

  bool supported = False;

  bt::Netwm::AtomList atoms;

  if (readSupported(win, atoms) && atoms.size() > 0) {
    if (find(atoms.begin(), atoms.end(), atom) != atoms.end()) {
      supported = True;
    }
  }

  return supported;
}

Window * Netclient::getNetVirtualRootList(Window win) {

  Atom type_ret;
  int format_ret;
  unsigned long nitems_ret, unused;
  unsigned char *data_ret;
  Window *winsReturn = 0;

  int e = XGetWindowProperty(_display, win, xaNetVirtualRoots(),
                             0, 0, False, XA_WINDOW, &type_ret,
                             &format_ret, &nitems_ret, &unused, &data_ret);
  
  if (e == Success && type_ret == XA_WINDOW && format_ret == 32) {
    Window *wins = (Window *) data_ret;
 
    winsReturn = new Window[nitems_ret];
    while (nitems_ret--) winsReturn[nitems_ret] = wins[nitems_ret];
  }

  if ( data_ret )
    XFree(data_ret);

  return winsReturn;

}
