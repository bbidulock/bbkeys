# not a GNU package. You can remove this line, if 
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 

SUBDIRS = src data doc
MAINTAINERCLEANFILES = aclocal.m4 config.h.in configure Makefile.in stamp-h.in

docdir = $(prefix)/doc/bbkeys
doc_DATA = AUTHORS BUGS ChangeLog LICENSE NEWS README TODO
EXTRA_DIST = $(doc_DATA) debian

dist-hook:
	rm -rf `find $(distdir) -name CVS`
	rm -f  `find $(distdir) -name ".\#*"`

## create Debian package
dist-deb:
	dpkg-buildpackage -rfakeroot -us -uc -tc
