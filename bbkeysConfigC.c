#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char bbkey_rcfile[130];

int setKeygrabs(void)
{

    int quit = 0, count = 0, j = 0;
    char Keytograb[80], Modifier[80], action[80];
    typedef struct {
	char tempLine[1025];
    } myLINE;
    typedef struct {
	myLINE myLine[20];
    } THEMOCKFILE;

#define OPTIONS "Minimize, Raise, Lower, Close, " \
            "\n\tWorkspace1, Workspace2, Workspace3, Workspace4, "\
            "\n\tNextWorkspace, PrevWorkspace, NextWindow, PrevWindow, "\
            "\n\tShadeWindow, MaximizeWindow, StickWindow, MaximizeVertical, "\
            "\n\tMaximizeHorizontal, BigNudgeRight, BigNudgeLeft, BigNudgeUp, "\
            "\n\tBigNudgeDown, HorizontalIncrement, VerticalIncrement, "\
	    "\n\tHorizontalDecrement, VerticalDecrement"


    FILE *menu_file;
    THEMOCKFILE MockFile;

    fprintf(stdout,
  "-[ Ye Olde bbkeys Keygrabber Conflaguration Device.  Now, honestly, why     ]-\n"
  "-[ you're using this instead of the gui bbkeysconf application I wrote is   ]-\n"
  "-[ beyond me. Do visit the bbkeys home page and  snarf this, won't you?     ]-\n"
  "-[ (http://movingparts.net)                                                 ]-\n\n"
  "-[ *** DISCLAIMER *** This tool will generate a COMPLETELY new ~/.bbkeysrc  ]-\n"
  "-[ config file for you, so if you actually by some odd coincidence do have  ]-\n"
  "-[ something that you want to keep in your ~/.bbkeysrc file, now might be   ]-\n"
  "-[ a good time to back it up before I go and overwrite the thing.  =:)      ]-\n"
    );

    while (!quit && count < 20) {
	memset(Keytograb, '\0', 80);
	memset(Modifier, '\0', 80);
	memset(action, '\0', 80);
	memset(MockFile.myLine[count].tempLine, 0, 1025);

	fprintf(stdout,
        "\n   ---------------------------------------------------------------------\n");
	fprintf(stdout, 
	"   Enter Key to Grab (Up,Down,R,L,etc.), or 'Q' to quit.  : ");
	fscanf(stdin, "%s", Keytograb);
	if (tolower(Keytograb[0]) == 'q') {
	    fprintf(stdout, "Okay, thanks for playing. g'bye.\n");
	    quit = 1;
	} else {
	    fprintf(stdout,
	"   Enter Modifier name (Control,Mod1,Shift+Mod1, etc.).   : ");
	    fscanf(stdin, "%s", Modifier);
	    fprintf(stdout, 
        "   Enter action to perform (%s)           : ", OPTIONS);
	    fscanf(stdin, "%s", action);
	    sprintf(MockFile.myLine[count].tempLine,
		    "KeyToGrab(%s), WithModifier(%s), " "WithAction(%s)\n",
		    Keytograb, Modifier, action);
	    count++;
	}
    }

    if (count <= 0)
	return 1;

    menu_file = fopen(bbkey_rcfile, "w");
    if (!menu_file) {
	fprintf(stderr, "bbkeys: Can't open menufile (%s) for writing. "
		"Can't really do a whole lot here, maynard.\n",
		bbkey_rcfile);
	return -1;
    } else {
	fprintf(stdout,
		"opened file ->%s<- for writing successfully.\n",
		bbkey_rcfile);
    }
    for (j = 0; j <= count; j++) {
	fprintf(menu_file, MockFile.myLine[j].tempLine);
    }
    fclose(menu_file);
    return 0;
}

int main(void)
{
    char *homedir;

    homedir = getenv("HOME");
    memset(bbkey_rcfile, 0, (sizeof(bbkey_rcfile)));

    sprintf(bbkey_rcfile, "%s/.bbkeysrc", homedir);

    exit(setKeygrabs());
}
