CPPFLAGS =	@CPPFLAGS@  @INTERLACE@ \
		-DGLOBAL_BB=\"$(datadir)/bbtools/bbkeys.bb\" \
		-DGLOBAL_NOBB=\"$(datadir)/bbtools/bbkeys.nobb\"

bin_PROGRAMS = bbkeys
bbkeys_SOURCES = bbkeys.cc Image.cc LinkedList.cc \
	bbkeys.hh Image.hh LinkedList.hh \
	Timer.hh Timer.cc \
	stackmenu.cc stackmenu.hh \
	main.cc main.hh \
	resource.cc resource.hh \
	Baseresource.cc Baseresource.hh \
	Basewindow.cc Basewindow.hh \
	BaseDisplay.cc BaseDisplay.hh \
	Basemenu.cc Basemenu.hh \
	wminterface.cc wminterface.hh \
	NETInterface.cc NETInterface.hh \
	grab_defs.hh blackboxstyle.hh
bbkeys_LDADD = @X_LIBS@

EXTRA_DIST = bbkeysconf.pl.in

bin_SCRIPTS = bbkeysconf.pl
