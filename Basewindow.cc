//  Basewindow.cc for bbtools.
//
//  Copyright (c) 1998-1999 by John Kennis, jkennis@chello.nl
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// (See the included file COPYING / GPL-2.0)
//
#include "Basewindow.hh"
#include "resource.hh"

#ifdef    HAVE_SIGNAL_H
#  include <signal.h>  
#endif // HAVE_SIGNAL_H

#ifdef    HAVE_SYS_SIGNAL_H
#  include <sys/signal.h>
#endif // HAVE_SYS_SIGNAL_H

#ifndef   SA_NODEFER
#  ifdef   SA_INTERRUPT
#    define SA_NODEFER SA_INTERRUPT
#  else // !SA_INTERRUPT
#    define SA_NODEFER (0)
#  endif // SA_INTERRUPT
#endif // SA_NODEFER


static int handleXErrors(Display *, XErrorEvent *);

Basewindow::Basewindow(int argc,char **argv,struct CMDOPTIONS *options)
  : BaseDisplay(argv[0], options->display_name) {
  iargc=argc;
  iargv=argv;
  config_filename=NULL;
  position=options->geometry;
  withdrawn=options->withdrawn;
  iconic=options->iconic;
  noQt=options->noQt;
  miniMe=options->miniMe;
  tinyMe=options->tinyMe;
  shape=options->shape;
  decorated=options->decorated;
  config_file=options->config_file;
  nobb_config=options->nobb_config;          
  current_screen_info = getScreenInfo(DefaultScreen(getXDisplay()));
  XSetErrorHandler((XErrorHandler) handleXErrors);

  wm_delete_window = XInternAtom (getXDisplay(), "WM_DELETE_WINDOW",False);
}

Basewindow::~Basewindow() {}

void Basewindow::process_event(XEvent *event)
{
}

Bool Basewindow::handleSignal(int sig) {
  switch (sig) {
    case SIGHUP:
     reconfigure();
    break;

    case SIGSEGV:
    case SIGFPE:
    case SIGINT:
    case SIGTERM:
      shutdown();

    default:
      return False;
  }

  return True;
}

void Basewindow::setupImageControl() {
  image_control = new BImageControl(this,getCurrentScreenInfo(),image_dither,
                                    colors_per_channel);
  image_control->installRootColormap();
}


// X error handler to handle any and all X errors while blackbox is running
static int handleXErrors(Display *d, XErrorEvent *e) {
#ifdef DEBUG  
  char errtxt[128];
  XGetErrorText(d, e->error_code, errtxt, 128);
  fprintf(stderr,
          "bbkeys:  [ X Error event received. ]\n"
          "  X Error of failed request:  %d %s\n"
          "  Major/minor opcode of failed request:  %d / %d\n"
          "  Resource id in failed request:  0x%lx\n", e->error_code, errtxt,
          e->request_code, e->minor_code, e->resourceid);
#endif
  return(False);
}

