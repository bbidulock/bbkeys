#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/keysym.h>

/********************************************************************/

Display *dpy;
Window root;

void		process_event();
void		eventLoop();

typedef struct {
	long keyName;
	long modMask;
	int action;
} KEY_ACTION_MAP;

typedef struct {
	int instructCount;
	KEY_ACTION_MAP KeyMap[30];
} KEY_GRAB_INSTRUCTION;

KEY_GRAB_INSTRUCTION grabSet;

#define grabIconify 1
#define grabRaise 2
#define grabLower 3
#define grabClose 4
#define grabShade 5
#define grabVertMax 6
#define grabHorizMax 7
#define grabFullMax 8

/********************************************************************/

int main(int argc, char *argv[])
{
	int i=0;
	grabSet.KeyMap[0].keyName=XK_m;
	grabSet.KeyMap[0].modMask=Mod1Mask;
	grabSet.KeyMap[0].action=grabIconify;
	grabSet.KeyMap[1].keyName=XK_Up;
	grabSet.KeyMap[1].modMask=Mod1Mask;
	grabSet.KeyMap[1].action=grabRaise;
	grabSet.KeyMap[2].keyName=XK_Down;
	grabSet.KeyMap[2].modMask=Mod1Mask;
	grabSet.KeyMap[2].action=grabLower;
	grabSet.KeyMap[3].keyName=XK_F4;
	grabSet.KeyMap[3].modMask=Mod1Mask;
	grabSet.KeyMap[3].action=grabClose;

	dpy = XOpenDisplay(NULL);
	root = DefaultRootWindow(dpy);

	for (i=0; i<4; i++) {
		XGrabKey(dpy, XKeysymToKeycode(dpy, grabSet.KeyMap[i].keyName), 
						grabSet.KeyMap[i].modMask, root,
						False, GrabModeAsync, GrabModeAsync);
	}
	grabSet.instructCount=4;


	eventLoop();
	exit(0);
}

/********************************************************************/

void process_event(XEvent *e) {
	int root_x, root_y;
	Window my_child, my_root;
	int win_x, win_y;
	int grabInt= -1, ifoo=0;
	unsigned int mask;
	unsigned int button;
	int revert_to=0;
	XEvent ce;

	switch (e->type) {
		case (KeyPress):
			XGetInputFocus(dpy, &my_child, &revert_to);
			for (ifoo=0; ifoo < grabSet.instructCount; ifoo++) {
				if ( XKeycodeToKeysym(dpy, e->xkey.keycode, 0)
					== (grabSet.KeyMap[ifoo].keyName) ) {
					grabInt = ifoo;
				}
			}
			if (grabInt > -1) {
				switch (grabSet.KeyMap[grabInt].action) {
					case grabIconify:
						XIconifyWindow(dpy, my_child, 0);
						break;
					case grabRaise:
						XRaiseWindow(dpy, my_child);
						break;
					case grabLower:
						XLowerWindow(dpy, my_child);
						break;
					case grabClose:
						ce.xclient.type = ClientMessage;
						ce.xclient.message_type = XInternAtom(dpy, "WM_PROTOCOLS", False);
						ce.xclient.display = dpy;
						/*ce.xclient.window = client.window;*/
						ce.xclient.window = my_child;
						ce.xclient.format = 32;
						ce.xclient.data.l[0] = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
						ce.xclient.data.l[1] = CurrentTime;
						ce.xclient.data.l[2] = 0l;
						ce.xclient.data.l[3] = 0l;
						ce.xclient.data.l[4] = 0l;
						XSendEvent(dpy, my_child, False, NoEventMask, &ce);
						/*XDestroyWindow(dpy, my_child);*/
						break;
				}
			}
		default:
			break;
	}
}

void eventLoop(void) {
	int xfd= ConnectionNumber(dpy);
	time_t lastTime = time(NULL);
	struct timeval tv;
	fd_set rfds;

	while (1) {
		if (XPending(dpy)) {
			XEvent e;
     	    XNextEvent(dpy, &e);
	        process_event(&e);

		}
		else {
			FD_ZERO(&rfds);
			FD_SET(xfd, &rfds);

			tv.tv_sec = 60;
			tv.tv_usec = 0;
					
			select(xfd + 1, &rfds, 0, 0, &tv);
		}
	}
}
