DEFAULT_RC=$(pkgdatadir)/bbkeysrc

AM_CPPFLAGS =	@CPPFLAGS@ -DDEFAULTRC=\"$(DEFAULT_RC)\"

bin_PROGRAMS = bbkeys
bbkeys_SOURCES = actions.cc \
	actions.hh \
	Config.cpp \
	Config.h \
	ConfigOptions.h \
	FileTokenizer.cpp \
	FileTokenizer.h \
	KeyClient.cpp \
	KeyClient.h \
	KeyGrabber.cpp \
	KeyGrabber.h \
	keytree.cc \
	keytree.hh \
	LocalUtil.h \
	main.cpp \
	Netclient.cpp \
	Netclient.h \
	ScreenHandler.cpp \
	ScreenHandler.h \
	WindowlistMenu.cpp \
	WindowlistMenu.h \
	window.cc \
	window.h



bbkeys_LDADD = @X_LIBS@

#EXTRA_DIST = bbkeysconf.pl.in

#bin_SCRIPTS = bbkeysconf.pl
