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

#include "bbkeys.hh"
#include "main.hh"

void Usage()
{
	fprintf(stderr, "\n%s version %s \n", BBTOOL, BBTOOL_VERSION);
	fprintf(stderr, "Usage: %s [options]\n", BBTOOL);
	fprintf(stderr, "Options:\n"
	" -display <display name>       X server to connect to\n"
	" -c[onfig] <filename>         Alternate 'look and feel' config file\n"
	" -rc[file] <filename>         Alternate keygrab definition file. \n"
	"                               (default is ~/.bbkeysrc)\n"
	" -n[obb]                      Fall back on default configuration\n"
	" -v[ersion]                   Display version number\n"
	" -h[elp]                      Display this help\n"
	" -geom[etry] <geometry>       Set geometry of window\n"
	" -d[ecorated]                 Show 'normal' decorated window\n"
	" -w[ithdrawn]                 Place bbtool in the Slit\n"
	" -i[conic]                    Start bbkeys in a minimized state\n"
	"                               (will override -withdrawn directive)\n"
	" -s[hape]                     Don't display groundplate\n"
	" -no[qt]                      Use non-qt configuration tool.\n"
	" -m[iniMe]                    Like Austin Powers 2. bbkeys, but smaller.\n"
	" -t[inyMe]                    All you can see is his keyhole *sniff*.\n\n");
}


int main(int argc, char **argv)
{
	int i;
	struct CMDOPTIONS options;
	options.display_name = NULL;
	options.geometry = NULL;
	options.withdrawn = False;
	options.iconic = False;
	options.shape = False;
	options.config_file = NULL;
	options.nobb_config = False;
	options.decorated = False;
	options.noQt = False;
	options.miniMe = False;
	options.tinyMe = False;
	options.bbkeysrc = NULL;

	for (i = 1; i < argc; i++) {
		if ((!strcmp(argv[i], "-display"))) {
			if (++i == argc) {
				Usage();
				exit(2);
			};
			options.display_name = argv[i];
		} else if ((!strcmp(argv[i], "-config")) |
			(!strcmp(argv[i], "-c"))) {
			if (++i == argc) {
				Usage();
				exit(2);
			};
			options.config_file = argv[i];
		} else if((!strcmp(argv[1], "-rcfile"))||
			(!strcmp(argv[i], "-rc"))){
			if(++i == argc){
				Usage();
				exit(2);
			};
			options.bbkeysrc=argv[i];
		} else if ((!strcmp(argv[i], "-nobb")) | (!strcmp(argv[i], "-n"))) {
			options.nobb_config = True;
		} else if ((!strcmp(argv[i], "-v"))
			|| (!strcmp(argv[i], "-version"))) {
			fprintf(stderr, " %s version %s\n", BBTOOL, BBTOOL_VERSION);
			exit(2);
		} else if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "-help"))) {
			Usage();
			exit(2);
		} else if ((!strcmp(argv[i], "-geometry"))
			|| (!strcmp(argv[i], "-geom"))) {
			if (++i == argc) {
				Usage();
				exit(2);
			};
			options.geometry = argv[i];
		} else if ((!strcmp(argv[i], "-withdrawn"))
			|| (!strcmp(argv[i], "-w"))) {
			options.withdrawn = True;
		} else if ((!strcmp(argv[i], "-iconic"))
			|| (!strcmp(argv[i], "-i"))) {
			options.iconic = True;
		} else if ((!strcmp(argv[i], "-shape"))
			|| (!strcmp(argv[i], "-s"))) {
			options.shape = True;
		} else if ((!strcmp(argv[i], "-decorated"))
			|| (!strcmp(argv[i], "-d"))) {
			options.decorated = True;
		} else if ((!strcmp(argv[i], "-noqt"))
			|| (!strcmp(argv[i], "-no"))) {
			options.noQt = True;
		} else if ((!strcmp(argv[i], "-miniMe"))
			|| (!strcmp(argv[i], "-m"))) {
			options.miniMe = True;
			options.tinyMe = False;
		} else if ((!strcmp(argv[i], "-tinyMe"))
			|| (!strcmp(argv[i], "-t"))) {
			options.tinyMe = True;
			options.miniMe = False;
		} else {
			Usage();
			exit(2);
		};
	
	}
	if (options.iconic) {
		options.withdrawn = False;
	}
	
	ToolWindow bbkeys(argc, argv, &options);
}
