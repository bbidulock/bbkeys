DEFAULT_RC=$(pkgdatadir)/bbkeysrc
DEFAULT_STYLE=$(pkgdatadir)/defaultStyle

AM_CPPFLAGS =	$(CPPFLAGS) \
	-DDEFAULTRC=\"$(DEFAULT_RC)\" \
	-DDEFAULTSTYLE=\"$(DEFAULT_STYLE)\"

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

bbkeys_LDADD = $(LIBS)

AM_INSTALLCHECK_STD_OPTIONS_EXEMPT = bbkeys
