// resource.cc for bbkeys - an tool for displaying and changing Blackbox
// workspaces.
//
//  Copyright (c) 1998-1999 John Kennis, j.m.b.m.kennis@tue.nl
//  Copyright (c) 2000-2001 Jason Kasper (vanRijn) <vr at movingparts dot  net>
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

#ifdef    HAVE_CONFIG_H
#  include "../config.h"
#endif													// HAVE_CONFIG_H

#ifdef   STDC_HEADERS
#  include <string.h>
#endif													// STDC_HEADERS

#if HAVE_STRINGS_H
# include <strings.h>
#endif

#ifdef   HAVE_STDIO_H
#  include <stdio.h>
#endif													// HAVE_STDIO_H

#include "resource.hh"
#include "blackboxstyle.hh"


Resource::Resource(ToolWindow * toolwindow):BaseResource(toolwindow)
{
	label.font = frame.font = menu.font=0;
	Load();
}

Resource::~Resource()
{
	Clean();
}

void Resource::Clean()
{
	if (label.font)
		XFreeFont(bbtool->getXDisplay(), label.font);
	if (frame.font)
		XFreeFont(bbtool->getXDisplay(), frame.font);
	if (menu.font)
		XFreeFont(bbtool->getXDisplay(), menu.font);
  frame.font=label.font=menu.font=0;				
}


void Resource::LoadBBToolResource(void)
{
	XrmValue value;
	char *value_type;

	if (XrmGetResource(resource_db, "bbkeys.autoConfig",
										 "Bbkeys.Autoconfig", &value_type, &value)) {
		if (!strncasecmp("true", value.addr, value.size)) {
			style.auto_config = True;
		} else {
			style.auto_config = False;
		}
	} else {
		style.auto_config = False;
	}

	if (XrmGetResource(resource_db, "bbkeys.honorModifiers",
										 "Bbkeys.Honormodifiers", &value_type, &value)) {
		if (!strncasecmp("true", value.addr, value.size)) {
			style.honor_modifiers = True;
		} else {
			style.honor_modifiers = False;
		}
	} else {
		style.honor_modifiers = False;
	}

	if (XrmGetResource(resource_db, "bbkeys.menu.stackedCycling",
										 "Bbkeys.Menu.StackedCycling", &value_type, &value)) {
		if (!strncasecmp("true", value.addr, value.size))
			menu.stackedCycling = True;
		else
			menu.stackedCycling = False;
	} else
		menu.stackedCycling = True;

	if (XrmGetResource(resource_db, "bbkeys.menu.showCycleMenu",
										 "Bbkeys.Menu.ShowCycleMenu", &value_type, &value)) {
		if (!strncasecmp("true", value.addr, value.size))
			menu.showCycleMenu = True;
		else
			menu.showCycleMenu = False;
	} else
		menu.showCycleMenu = True;

	if (XrmGetResource(resource_db, "bbkeys.menu.showAllWorkspaces",
										 "Bbkeys.Menu.ShowAllWorkspaces", &value_type, &value)) {
		if (!strncasecmp("true", value.addr, value.size))
			menu.showAll = True;
		else
			menu.showAll = False;
	} else
		menu.showAll = False;

	Frame();

	Menu();

	SizeAndPosition();

	Label();

	Button();

}


void Resource::Frame()
{
	XrmValue value;
	char *value_type;
	char *color;
	char *gradient;

	if (XrmGetResource
			(resource_db, "toolbar.color", "Toolbar.Color", &value_type,
			 &value)) {
		color = strdup(value.addr);
	} else {
		color = strdup("black");
	}

	if (XrmGetResource
			(resource_db, "toolbar", "Toolbar", &value_type, &value)) {
		gradient = strdup(value.addr);
	} else {
		gradient = strdup("Raised Gradient Vertical Bevel1");
	}


	readTexture("bbkeys.frame", "Bbkeys.Frame", BB_FRAME,
							BB_FRAME2, color, color, gradient, &frame.texture);

	if (XrmGetResource
			(resource_db, "bbkeys.bevelWidth", "Bbkeys.BevelWidth",
			 &value_type, &value)) {
		if (sscanf(value.addr, "%u", &frame.bevelWidth) != 1)
			frame.bevelWidth = 4;
		else if (frame.bevelWidth == 0)
			frame.bevelWidth = 4;
	} else
		 if (XrmGetResource
				 (resource_db, BB_BEVELWIDTH, BB_BEVELWIDTH2, &value_type, &value)) {
		if (sscanf(value.addr, "%u", &frame.bevelWidth) != 1)
			frame.bevelWidth = 4;
		else if (frame.bevelWidth == 0)
			frame.bevelWidth = 4;
	} else
		frame.bevelWidth = 4;

	// strdup does a malloc to achieve its results, so make sure we free()
	// afterwards
	free(color);
	free(gradient);

}


void Resource::SizeAndPosition()
{
	XrmValue value;
	char *value_type;
	unsigned int w, h;
	char positionstring[11];

	if (!(bbtool->position)) {
		if (!
				(XrmGetResource
				 (resource_db, "bbkeys.position", "Bbkeys.Position",
					&value_type, &value)))
			strncpy(positionstring, "-0-0", 5);
		else
			strncpy(positionstring, value.addr, strlen(value.addr) + 1);
	} else
		strncpy(positionstring, bbtool->position,
						strlen(bbtool->position) + 1);


	position.mask =
		 XParseGeometry(positionstring, &position.x, &position.y, &w, &h);

	if (!(position.mask & XValue))
		position.x = 0;
	if (!(position.mask & YValue))
		position.y = 0;


	if (XrmGetResource(resource_db, "bbkeys.columns",
										 "Bbkeys.Columns", &value_type, &value)) {
		if (sscanf(value.addr, "%u", &columns) != 1)
			columns = 1;
		else {
			position.vertical = True;
			if (columns == 0)
				columns = 1;
		}
	} else
		columns = 1;

	if (XrmGetResource(resource_db, "bbkeys.rows", "Bbkeys.Rows",
										 &value_type, &value)) {
		if (sscanf(value.addr, "%u", &rows) != 1)
			rows = 1;
		else {
			position.horizontal = True;
			if (rows == 0)
				rows = 1;
		}
	} else
		rows = 1;

	if (!position.horizontal && !position.vertical) {
		if (bbtool->withdrawn)
			position.vertical = True;
		else
			position.horizontal = True;
	}

	if (!(XrmGetResource(resource_db, "bbkeys.desktop.width",
											 "Bbkeys.Desktop.Width", &value_type, &value))) {
		if (!bbtool->withdrawn)
			desktopSize.width = 40;
		else
			desktopSize.width =
				 64 / columns - ((columns - 1)) * frame.bevelWidth;
	} else if (sscanf(value.addr, "%u", &desktopSize.width) != 1) {
		if (!bbtool->withdrawn)
			desktopSize.width = 40;
		else
			desktopSize.width =
				 64 / columns - ((columns - 1)) * frame.bevelWidth;

	}

	if (!(XrmGetResource(resource_db, "bbkeys.desktop.height",
											 "Bbkeys.Desktop.Height", &value_type, &value))) {
		if (!bbtool->withdrawn)
			desktopSize.height = 30;
		else
			desktopSize.height = 48 / columns;
	} else if (sscanf(value.addr, "%u", &desktopSize.height) != 1) {
		if (!bbtool->withdrawn)
			desktopSize.width = 30;
		else
			desktopSize.height = 48 / columns;
	}

	/* need this to compute the height */
	const char *defaultFont =
		 /* "-*-helvetica-medium-r-*-*-*-120-*-*-*-*-*-*"; */
		 "-*-helvetica-medium-r-*-*-*-100-*-*-*-*-*-*";

	if (frame.font) {
		XFreeFont(bbtool->getXDisplay(), frame.font);
		frame.font = 0;
	}

	if (XrmGetResource // try to load TitleFont, which will pick up *Font
				 (resource_db, BB_FONT, BB_FONT2, &value_type, &value)) {
		if ((frame.font =
				 XLoadQueryFont(bbtool->getXDisplay(), value.addr)) == NULL) {
			fprintf(stderr,
							" bbkeys: couldn't load font '%s'\n"
							" ...  reverting to default font.", value.addr);
			if ((frame.font =
					 XLoadQueryFont(bbtool->getXDisplay(), defaultFont)) == NULL) {
				fprintf(stderr,
								"bbkeys: couldn't load default font.  please check to\n"
								"make sure the necessary font is installed '%s'\n",
								defaultFont);
				exit(2);
			}
		}
	} else if (XrmGetResource // try to load toolbar.font
				 (resource_db, "toolbar.font", "Toolbar.Font", &value_type, &value)) {
		if ((frame.font =
				 XLoadQueryFont(bbtool->getXDisplay(), value.addr)) == NULL) {
			fprintf(stderr,
							" bbkeys: couldn't load font '%s'\n"
							" ...  reverting to default font.", value.addr);
			if ((frame.font =
					 XLoadQueryFont(bbtool->getXDisplay(), defaultFont)) == NULL) {
				fprintf(stderr,
								"bbkeys: couldn't load default font.  please check to\n"
								"make sure the necessary font is installed '%s'\n",
								defaultFont);
				exit(2);
			}
		}
	} else if (XrmGetResource
			(resource_db, "bbkeys.heightBy.font", "Bbkeys.heightBy.Font",
			 &value_type, &value)) {
		if ((frame.font =
				 XLoadQueryFont(bbtool->getXDisplay(), value.addr)) == NULL) {
			fprintf(stderr,
							" bbkeys: couldn't load font '%s'\n"
							" ...  reverting to default font.", value.addr);
			if ((frame.font =
					 XLoadQueryFont(bbtool->getXDisplay(), defaultFont)) == NULL) {
				fprintf(stderr,
								"bbkeys: couldn't load default font.  please check to\n"
								"make sure the necessary font is installed '%s'\n",
								defaultFont);
				exit(2);
			}
		}
	} else {
		if ((frame.font =
				 XLoadQueryFont(bbtool->getXDisplay(), defaultFont)) == NULL) {
			fprintf(stderr,
							"bbkeys: couldn't load default font.  please check to\n"
							"make sure the necessary font is installed '%s'\n",
							defaultFont);
			exit(2);
		}
	}
}

void Resource::Label(void)
{

	XrmValue value;
	char *value_type;

	/* text-label resources */
	if (XrmGetResource(resource_db, "bbkeys.label.transparent",
										 "Bbkeys.label.Transparent", &value_type, &value)) {
		if (!strncasecmp("true", value.addr, value.size))
			label.transparent = True;
		else
			label.transparent = False;
	} else
		label.transparent = False;

	readTexture("bbkeys.label", "Bbkeys.Label", BB_LABEL,
							BB_LABEL2, "slategrey", "darkslategrey",
							"Sunken Gradient Diagonal Bevel1", &label.texture);

	readColor("bbkeys.textColor", "Bbkeys.TextColor",
						BB_LABEL_TEXTCOLOR, BB_LABEL_TEXTCOLOR2,
						"LightGrey", &label.textColor);


	const char *defaultFont = "-*-helvetica-medium-r-*-*-*-100-*-*-*-*-*-*";

	if (label.font) {
		XFreeFont(bbtool->getXDisplay(), label.font);
		label.font = 0;
	}

	if (XrmGetResource
				 (resource_db, BB_FONT, BB_FONT2, &value_type, &value)) {
		if ((label.font =
				 XLoadQueryFont(bbtool->getXDisplay(), value.addr)) == NULL) {
			fprintf(stderr,
							" bbkeys: couldn't load font '%s'\n"
							" ...  reverting to default font.", value.addr);
			if ((label.font =
					 XLoadQueryFont(bbtool->getXDisplay(), defaultFont)) == NULL) {
				fprintf(stderr,
								"bbkeys: couldn't load default font.  please check to\n"
								"make sure the necessary font is installed '%s'\n",
								defaultFont);
				exit(2);
			}
		}
	} else if (XrmGetResource // try to load toolbar.font
				 (resource_db, "toolbar.font", "Toolbar.Font", &value_type, &value)) {
		if ((label.font =
				 XLoadQueryFont(bbtool->getXDisplay(), value.addr)) == NULL) {
			fprintf(stderr,
							" bbkeys: couldn't load font '%s'\n"
							" ...  reverting to default font.", value.addr);
			if ((label.font =
					 XLoadQueryFont(bbtool->getXDisplay(), defaultFont)) == NULL) {
				fprintf(stderr,
								"bbkeys: couldn't load default font.  please check to\n"
								"make sure the necessary font is installed '%s'\n",
								defaultFont);
				exit(2);
			}
		}
	} else if (XrmGetResource
			(resource_db, "bbkeys.label.font", "Bbkeys.Label.Font",
			 &value_type, &value)) {
		if ((label.font =
				 XLoadQueryFont(bbtool->getXDisplay(), value.addr)) == NULL) {
			fprintf(stderr,
							" bbkeys: couldn't load font '%s'\n"
							" ...  reverting to default font.", value.addr);
			if ((label.font =
					 XLoadQueryFont(bbtool->getXDisplay(), defaultFont)) == NULL) {
				fprintf(stderr,
								"bbkeys: couldn't load default font.  please check to\n"
								"make sure the necessary font is installed '%s'\n",
								defaultFont);
				exit(2);
			}
		}
	} else {
		if ((label.font =
				 XLoadQueryFont(bbtool->getXDisplay(), defaultFont)) == NULL) {
			fprintf(stderr,
							"bbkeys: couldn't load default font.  please check to\n"
							"make sure the necessary font is installed '%s'\n",
							defaultFont);
			exit(2);
		}
	}
}

void Resource::Button()
{
	readTexture("bbkeys.button", "Bbkeys.Button",
							BB_BUTTON, BB_BUTTON2,
							"rgb:4/7/8", "rgb:2/2/2",
							"Raised Gradient Diagonal Bevel1", &button.texture);

	readTexture("bbkeys.button.pressed", "Bbkeys.Button.Pressed",
							BB_BUTTON_PRESSED, BB_BUTTON_PRESSED2,
							"rgb:4/4/4", "rgb:c/c/c",
							"Sunken Gradient Diagonal Bevel1", &button.texture_pressed);

}

void Resource::Menu()
{
	XrmValue value;
	char *value_type;

	readTexture("bbkeys.menu.frame","Bbkeys.Menu.Frame",BB_MENU_TEXTURE,BB_MENU_TEXTURE2,
              "slategrey","darkslategrey",
              "Raised Diagonal Gradient Bevel1",&menu.texture);
															   	
	readTexture("bbkeys.menu.hilight","Bbkeys.Menu.Hilight",
					BB_MENU_HILITE_TEXTURE,BB_MENU_HILITE_TEXTURE2,
              "darkslategrey","slategrey",
              "Raised Elliptic Gradient Bevel2",&menu.hiTexture);
															   	
  readColor("bbkeys.menu.highlight.color",
				    "Bbkeys.Menu.Highlight.Color",
             BB_MENU_HIGHLIGHT_COLOR,BB_MENU_HIGHLIGHT_COLOR2,
             "rgb:c/9/6",&menu.highlightColor);
  
	readColor(
		    BB_MENU_TEXTCOLOR,BB_MENU_TEXTCOLOR2,
			"bbkeys.menu.textColor","Bbkeys.Menu.TextColor",
                    "LightGrey",&menu.textColor);

	readColor(
 			BB_MENU_HITEXTCOLOR,BB_MENU_HITEXTCOLOR2,
			"bbkeys.menu.highlight.textColor", "Bbkeys.Menu.Highlight.TextColor",
            "white",&menu.hiTextColor);

  if (XrmGetResource(resource_db,"bbkeys.menu.justify","Bbkeys.Menu.Justify",
						&value_type, &value))
	{
    	if (! strncasecmp("left", value.addr, value.size))
	      menu.justify = LeftJustify;
    	else if (! strncasecmp("right", value.addr, value.size))
	      menu.justify = RightJustify;
	    else if (! strncasecmp("center", value.addr, value.size))
	      menu.justify = CenterJustify;
	    else
    	  menu.justify = LeftJustify;
	  }
	  else if (XrmGetResource(resource_db,BB_MENU_JUSTIFY,BB_MENU_JUSTIFY2,
								&value_type, &value))
	  {
    	if (! strncasecmp("left", value.addr, value.size))
	      menu.justify = LeftJustify;
    	else if (! strncasecmp("right", value.addr, value.size))
	      menu.justify = RightJustify;
	    else if (! strncasecmp("center", value.addr, value.size))
	      menu.justify = CenterJustify;
	    else
    	  menu.justify = LeftJustify;	
	  }
	  else
	    menu.justify = LeftJustify;
    
    if (XrmGetResource(resource_db, "bbkeys.menu.bulletStyle", 
                       "Bbkeys.Menu.BulletStyle", &value_type, &value)) {
    if (! strncasecmp(value.addr, "empty", value.size))
      menu.bullet_style = Empty;
    else if (! strncasecmp(value.addr, "square", value.size))
      menu.bullet_style = Square;
    else if (! strncasecmp(value.addr, "triangle", value.size))
      menu.bullet_style = Triangle;
    else if (! strncasecmp(value.addr, "diamond", value.size))
      menu.bullet_style = Diamond;
    else
      menu.bullet_style = Round;
  } else {
    if (XrmGetResource(resource_db, BB_MENU_BULLETSTYLE, 
                       BB_MENU_BULLETSTYLE2, &value_type, &value)) {
      if (! strncasecmp(value.addr, "empty", value.size))
        menu.bullet_style = Empty;
      else if (! strncasecmp(value.addr, "square", value.size))
        menu.bullet_style = Square;
      else if (! strncasecmp(value.addr, "triangle", value.size))
        menu.bullet_style = Triangle;
      else if (! strncasecmp(value.addr, "diamond", value.size))
        menu.bullet_style = Diamond;
      else
        menu.bullet_style = Round;
    } else
      menu.bullet_style = Round;
  }
    
  if (XrmGetResource(resource_db, "bbkeys.menu.bulletPosition",
                     "Bbkeys.Menu.BulletPosition", &value_type, &value)) {
    if (! strncasecmp(value.addr, "right", value.size))
      menu.bullet_pos = Right;
    else
      menu.bullet_pos = Left;
  } else {
    if (XrmGetResource(resource_db, BB_MENU_BULLETPOSITION,
                       BB_MENU_BULLETPOSITION2, &value_type, &value)) {
      if (! strncasecmp(value.addr, "right", value.size))
        menu.bullet_pos = Right;
      else
        menu.bullet_pos = Left;
    } else
      menu.bullet_pos = Left;
  }

	const char *defaultFont = "-*-helvetica-medium-r-*-*-*-120-*-*-*-*-*-*";
	
	if (menu.font)
	{
		XFreeFont(bbtool->getXDisplay(), menu.font);
		menu.font = 0;
	}
	
if  (XrmGetResource(resource_db, BB_MENU_FONT, BB_MENU_FONT2, &value_type, &value))
	{
		if ((menu.font = XLoadQueryFont(bbtool->getXDisplay(), value.addr)) == NULL)
		{
			fprintf(stderr, " blackbox: couldn't load font '%s'\n"
	      					" ...  reverting to default font.", value.addr);
			if ((menu.font = XLoadQueryFont(bbtool->getXDisplay(), defaultFont)) == NULL)
			{
				fprintf(stderr,
					"blackbox: couldn't load default font.  please check to\n"
					"make sure the necessary font is installed '%s'\n",
					defaultFont);
				exit(2);
			}
		}
	} else if (XrmGetResource(resource_db, "bbkeys.menu.font", "Bbkeys.Menu.Font",
						&value_type, &value))
	{
		if ((menu.font = XLoadQueryFont(bbtool->getXDisplay(), value.addr)) == NULL)
		{
    	fprintf(stderr, " blackbox: couldn't load font '%s'\n"
	      							" ...  reverting to default font.", value.addr);
		    if ((menu.font = XLoadQueryFont(bbtool->getXDisplay(), defaultFont)) == NULL)
			{
				fprintf(stderr,
					"blackbox: couldn't load default font.  please check to\n"
					"make sure the necessary font is installed '%s'\n",
					defaultFont);
			exit(2);
			}
		}
	} else {
		if ((menu.font = XLoadQueryFont(bbtool->getXDisplay(), defaultFont)) == NULL)
		{
			fprintf(stderr,"blackbox: couldn't load default font.  please check to\n"
				    "make sure the necessary font is installed '%s'\n", defaultFont);
			exit(2);
		}
	}	
}
