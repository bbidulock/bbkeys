// Baseresource.cc for bbtools - tools to display resources in X11.
//
//  Copyright (c) 1998-1999 John Kennis, jkennis@chello.nl
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

#include <stdlib.h>
#include "bbkeys.hh"
#include "Baseresource.hh"
#include "blackboxstyle.hh"

BaseResource::BaseResource(ToolWindow *toolwindow) {
  XrmValue value;
  char *value_type;

  bbtool=toolwindow;
  style.style_filename=NULL;
  style.conf_filename=NULL;
  
  if (bbtool->nobb_config) {
      ReadBBtoolResource();
      ResourceType=BBTOOLS;
  } else {
    char *homedir = getenv("HOME");
    bbtool->config_filename = new char[strlen(homedir) + 32];
    sprintf(bbtool->config_filename, "%s/.blackboxrc", homedir);
    if ((resource_db = XrmGetFileDatabase(bbtool->config_filename))!=NULL) {
      ReadBlackboxResource();
      ResourceType=BLACKBOX;
    }
    else {
      ReadBBtoolResource();
      ResourceType=BBTOOLS;
    }
  }

  if (XrmGetResource(resource_db, "session.colorsPerChannel",
                     "Session.ColorsPerChannel", &value_type, &value)) {
    if (sscanf(value.addr, "%d", &bbtool->colors_per_channel) != 1) {
      bbtool->colors_per_channel = 4;
    } else {
      if (bbtool->colors_per_channel < 2) bbtool->colors_per_channel = 2;
      if (bbtool->colors_per_channel > 6) bbtool->colors_per_channel = 6;
    }
  } else
    bbtool->colors_per_channel = 4;


  if (XrmGetResource(resource_db, "session.imageDither",
                     "Session.ImageDither", &value_type, &value)) {
    if (! strncasecmp("true", value.addr, value.size))
      bbtool->image_dither = True;
    else
      bbtool->image_dither = False;
  } else
    bbtool->image_dither = True;

 if (bbtool->image_dither && 
     bbtool->getCurrentScreenInfo()->getVisual()->c_class == TrueColor 
     && bbtool->getCurrentScreenInfo()->getDepth() >= 24)
    bbtool->image_dither = False;
  
	/* Need to do this here */
	bbtool->setupImageControl();

  if (XrmGetResource(resource_db, "bbkeys.autoConfig",
                     "Bbkeys.Autoconfig", &value_type, &value)) {
    if (! strncasecmp("true", value.addr, value.size)) {
      auto_config = True;
    } else
      auto_config = False;
  } else
    auto_config = False;

  if (XrmGetResource(resource_db, "bbkeys.honorModifiers",
                     "Bbkeys.Honormodifiers", &value_type, &value)) {
    if (! strncasecmp("true", value.addr, value.size)) {
      bbtool->honor_modifiers = True;
    } else
      bbtool->honor_modifiers = False;
  } else
    bbtool->honor_modifiers = False;

  if (bbtool->withdrawn) auto_config=False;

  if (XrmGetResource(resource_db, "bbkeys.autoConfig.checkTimeout",
                     "Bbkeys.Autoconfig.CheckTimeout", &value_type, &value)) {
    if (sscanf(value.addr, "%u", &check_timeout) != 1)
      check_timeout = 10;
  }
  else
    check_timeout = 10;

  if ((bbtool->config_filename!=NULL)&(auto_config)) {
    if (stat(bbtool->config_filename,&file_status)!=0) {
      fprintf(stderr,"Can't use autoconfig");
      auto_config=false;
      mtime=0;
    } else {
      mtime=file_status.st_mtime;
  
      timer=new BTimer(toolwindow->getCurrentScreenInfo()->getBaseDisplay(),
                        this);
      timer->setTimeout(1000*check_timeout);
      timer->start();
    }
  }

}

void BaseResource::Load() {

  LoadBBToolResource();
  XrmDestroyDatabase(resource_db);
}

BaseResource::~BaseResource() {
  delete [] style.style_filename;
  delete [] style.conf_filename;

  style.style_filename=NULL;
  style.conf_filename=NULL;
}

void BaseResource::timeout() {
  if (stat(bbtool->config_filename,&file_status)!=0) {
    fprintf(stderr,"Autoconfig error: Cannot get status of:%s\n",
            bbtool->config_filename);
    style.mtime=0;
  } else {
    if (mtime!=file_status.st_mtime) {
      bbtool->reconfigure();
      mtime=file_status.st_mtime;
    }
  }
}

void BaseResource::CopyColor(BColor *Color1,BColor *Color2) {
  Color2->setPixel(Color1->getPixel());
  Color2->setRGB(Color1->getRed(),Color1->getGreen(),Color1->getBlue());
}

void BaseResource::CopyTexture(BTexture Texture1,BTexture *Texture2) {
  CopyColor(Texture1.getColor(),Texture2->getColor());
  CopyColor(Texture1.getColorTo(),Texture2->getColorTo());
  CopyColor(Texture1.getHiColor(),Texture2->getHiColor());
  CopyColor(Texture1.getLoColor(),Texture2->getLoColor());
  Texture2->setTexture(Texture1.getTexture());
}

void BaseResource::Reload() {
  struct stat file_status;

  switch (ResourceType) {
    case BBTOOLS:
      {
        ReadBBtoolResource();
      }
      break;
    case BLACKBOX:
      {
        if ((resource_db = XrmGetFileDatabase(bbtool->config_filename))!=NULL)
          ReadBlackboxResource();
      }
      break;
  }
  LoadBBToolResource();

  if ((bbtool->config_file!=NULL) && (style.auto_config)) {
    if (stat(bbtool->config_filename,&file_status)!=0) {
      fprintf(stderr,"Can't use autoconfig");
      style.auto_config=false;
      mtime=0;
    } else
        mtime=file_status.st_mtime;
  }

  XrmDestroyDatabase(resource_db);
}

bool BaseResource::ReadResourceFromFilename(char *rname, char *rclass) {
  struct stat file_status;
  char *filename=NULL;
  XrmValue value;
  char *value_type;

  if (XrmGetResource(resource_db,rname,rclass, &value_type, &value)) {
    int len = strlen(value.addr);
    delete [] filename;
    filename = new char[len + 1];
    memset(filename, 0, len + 1);
    strncpy(filename, value.addr, len);
    if (stat(filename,&file_status)!=0) {
      db=NULL;
      delete [] filename;
      return(False);
    }
    db = XrmGetFileDatabase(filename);
    delete [] filename;
    return(True);
  }
  db=NULL;
  delete [] filename;
  return(False);
}

void BaseResource::ReadBBtoolResource() {

  if (bbtool->config_file) {
    if ((resource_db = XrmGetFileDatabase(bbtool->config_file))==NULL) {
      fprintf(stderr,"Could not open config file: %s\n",
                bbtool->config_file);
      fprintf(stderr,"Using internal defaults.\n");
    }
    else
      bbtool->config_filename=bbtool->config_file;
  }
  else {
    delete [] style.conf_filename;
    char *homedir = getenv("HOME");
    int len=strlen(homedir) + strlen(BBTOOL_LOCAL);

    style.conf_filename = new char[len+2];
    memset(style.conf_filename, 0, len + 2);
    sprintf(style.conf_filename, "%s/%s", homedir,BBTOOL_LOCAL);
    if ((resource_db = XrmGetFileDatabase(style.conf_filename))==NULL) {
      delete [] style.conf_filename;
      int len=strlen(GLOBAL_NOBB);
      style.conf_filename = new char[len + 1];
      memset(style.conf_filename, 0, len + 1);
      strncpy(style.conf_filename,GLOBAL_NOBB, len);
      if ((resource_db = XrmGetFileDatabase(style.conf_filename))==NULL) {
        fprintf(stderr,"Could not open default config file: %s\n",
                style.conf_filename);
        fprintf(stderr,"Using internal defaults.\n");
      }
      else
        bbtool->config_filename=bbtool->config_file;
    }
    else
      bbtool->config_filename=bbtool->config_file;
  }  
  
}

void BaseResource::ReadBlackboxResource() {

  if (!ReadResourceFromFilename("session.styleFile","Session.StyleFile")) {
    fprintf(stderr,"Could not open blackbox style file\n");
  } else
    XrmCombineDatabase(db,&resource_db,False);

  if (bbtool->config_file!=NULL) {
    if ((db = XrmGetFileDatabase(bbtool->config_file))==NULL) {
      fprintf(stderr,"Could not open config file: %s\n",
              bbtool->config_file);
      return;
    } else {
      XrmCombineDatabase(db,&resource_db,True);
    }
  } else {
    delete [] style.conf_filename;
    char *homedir = getenv("HOME");
    int len=strlen(homedir) + strlen(BLACKBOX_LOCAL); 
    style.conf_filename = new char[len+2];
    memset(style.conf_filename, 0, len + 2);
    sprintf(style.conf_filename, "%s/%s", homedir,BLACKBOX_LOCAL);
    if ((db = XrmGetFileDatabase(style.conf_filename))==NULL) {
      delete [] style.conf_filename;
      int len=strlen(GLOBAL_BB);
      style.conf_filename = new char[len + 1];
      memset(style.conf_filename, 0, len + 1);
      strncpy(style.conf_filename,GLOBAL_BB, len);
      if ((db = XrmGetFileDatabase(style.conf_filename))==NULL) {
        fprintf(stderr,"Could not open default config file: %s\n",
                style.conf_filename);
        fprintf(stderr,"Using internal defaults.\n");
        return;
      }
      else XrmCombineDatabase(db,&resource_db,True);
    }
    else XrmCombineDatabase(db,&resource_db,True);
  }
}

void BaseResource::readTexture(char *rname,char *rclass, char *bbname,
                               char *bbclass,char *dcolor,char *dcolorTo,
                               char *dtexture,BTexture *texture) {
  readDatabaseTexture(rname,rclass,texture);
  if (!texture->getTexture()) {
    readDatabaseTexture(bbname,bbclass,texture);
    if (!texture->getTexture()) {
      bbtool->getImageControl()->parseTexture(texture, dtexture);
      bbtool->getImageControl()->parseColor(texture->getColor(), dcolor);
      bbtool->getImageControl()->parseColor(texture->getColorTo(), dcolorTo);
    }
  }
}

void BaseResource::readColor(char *rname,char *rclass, char *bbname,
                             char *bbclass,char *dcolor,BColor *color) {
  readDatabaseColor(rname,rclass,color);
  if (!color->isAllocated()) {
    readDatabaseColor(bbname,bbclass,color);
    if (!color->isAllocated()) 
      bbtool->getImageControl()->parseColor(color,dcolor);
  }
}

void BaseResource::readDatabaseTexture(char *rname, char *rclass,
				  BTexture *texture) {
  XrmValue value;
  char *value_type;

  texture->setTexture(0);
  
  if (XrmGetResource(resource_db, rname, rclass, &value_type,
		     &value))
    bbtool->getImageControl()->parseTexture(texture, value.addr);
  
  if (texture->getTexture() & BImage_Solid) {
    int clen = strlen(rclass) + 8, nlen = strlen(rname) + 8;
    char *colorclass = new char[clen], *colorname = new char[nlen];
    
    sprintf(colorclass, "%s.Color", rclass);
    sprintf(colorname,  "%s.color", rname);
    
    readDatabaseColor(colorname, colorclass, texture->getColor());
    
    delete [] colorclass;
    delete [] colorname;
    
    if ((! texture->getColor()->isAllocated()) ||
	(texture->getTexture() & BImage_Flat))
      return;
    
    XColor xcol;
    
    xcol.red = (unsigned int) (texture->getColor()->getRed() +
			       (texture->getColor()->getRed() >> 1));
    if (xcol.red >= 0xff) xcol.red = 0xffff;
    else xcol.red *= 0xff;
    xcol.green = (unsigned int) (texture->getColor()->getGreen() +
				 (texture->getColor()->getGreen() >> 1));
    if (xcol.green >= 0xff) xcol.green = 0xffff;
    else xcol.green *= 0xff;
    xcol.blue = (unsigned int) (texture->getColor()->getBlue() +
				(texture->getColor()->getBlue() >> 1));
    if (xcol.blue >= 0xff) xcol.blue = 0xffff;
    else xcol.blue *= 0xff;
    
    if (! XAllocColor(bbtool->getXDisplay(), bbtool->getImageControl()->getColormap(),
                      &xcol))
      xcol.pixel = 0;
    
    texture->getHiColor()->setPixel(xcol.pixel);
    
    xcol.red =
      (unsigned int) ((texture->getColor()->getRed() >> 2) +
		      (texture->getColor()->getRed() >> 1)) * 0xff;
    xcol.green =
      (unsigned int) ((texture->getColor()->getGreen() >> 2) +
		      (texture->getColor()->getGreen() >> 1)) * 0xff;
    xcol.blue =
      (unsigned int) ((texture->getColor()->getBlue() >> 2) +
		      (texture->getColor()->getBlue() >> 1)) * 0xff;
    
    if (! XAllocColor(bbtool->getXDisplay(), bbtool->getImageControl()->getColormap(),
                      &xcol))
      xcol.pixel = 0;
    
    texture->getLoColor()->setPixel(xcol.pixel);
  } else if (texture->getTexture() & BImage_Gradient) {
    int clen = strlen(rclass) + 10, nlen = strlen(rname) + 10;
    char *colorclass = new char[clen], *colorname = new char[nlen],
      *colortoclass = new char[clen], *colortoname = new char[nlen];
    
    sprintf(colorclass, "%s.Color", rclass);
    sprintf(colorname,  "%s.color", rname);
    
    sprintf(colortoclass, "%s.ColorTo", rclass);
    sprintf(colortoname,  "%s.colorTo", rname);
    
    readDatabaseColor(colorname, colorclass, texture->getColor());
    readDatabaseColor(colortoname, colortoclass, texture->getColorTo());
    
    delete [] colorclass;
    delete [] colorname;
    delete [] colortoclass;
    delete [] colortoname;
  }
}

void BaseResource::readDatabaseColor(char *rname, char *rclass, BColor *color) {
  XrmValue value;
  char *value_type;
  if (XrmGetResource(resource_db, rname, rclass, &value_type,
		     &value))
    bbtool->getImageControl()->parseColor(color, value.addr);
  else
    // parsing with no color string just deallocates the color, if it has
    // been previously allocated
    bbtool->getImageControl()->parseColor(color);
}

