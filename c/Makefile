CC       = gcc
CFLAGS   = -O2 -Wall# -g
XROOT    = /usr/X11R6
XAWPATH  = -I$(XROOT)/include -L$(XROOT)/lib
XAWLIBS  = -lXaw -lX11
GTKPATH  = `gtk-config --cflags`
GTKLIBS  = `gtk-config --libs`

XAWPROGS = xaw-panel xaw-palette bbkeys
GTKPROGS = gtk-panel gtk-palette gtk-palette2 

all: $(XAWPROGS) $(GTKPROGS)

$(XAWPROGS): %: %.c
	$(CC) $(XAWPATH) $(XAWLIBS) $< -o $@

$(GTKPROGS): %: %.c
	$(CC) $(GTKPATH) $(GTKLIBS) $< -o $@

clean:
	rm -f $(XAWPROGS) $(GTKPROGS)
