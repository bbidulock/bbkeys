DEFAULT_RC=$(pkgdatadir)/bbkeysrc
DEFAULT_STYLE=$(pkgdatadir)/defaultStyle

AM_CXXFLAGS = $(X11_CFLAGS) $(XEXT_CFLAGS) $(XFT_CFLAGS) $(LIBBT_CFLAGS)
AM_LDFLAGS = $(LIBBT_LIBS) $(XFT_LIBS) $(XEXT_LIBS) $(X11_LIBS)
AM_CPPFLAGS = -DDEFAULTRC=\"$(DEFAULT_RC)\" -DDEFAULTSTYLE=\"$(DEFAULT_STYLE)\"

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
	main.h \
	Netclient.cpp \
	Netclient.h \
	ScreenHandler.cpp \
	ScreenHandler.h \
	window.cc \
	window.hh \
	WindowlistMenu.cpp \
	WindowlistMenu.h 
bbkeys_LDADD = /usr/lib/libbt.a

AM_INSTALLCHECK_STD_OPTIONS_EXEMPT = bbkeys
