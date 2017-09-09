EXTRA_DIST = bbkeys.spec.in

CLEANFILES = bbkeys.spec \
	BUILD SPECS RPMS SOURCES SRPMS \
	rpmrc rpmmacros

dist-rpm:
	$(MAKE) clean
	mkdir -p BUILD SPECS RPMS/{noarch,i386,i686} SOURCES SRPMS
	cp -f ${top_srcdir}/@PACKAGE@-@VERSION@.tar.gz ./SOURCES
	echo "%_topdir ${PWD}"	> rpmmacros
	echo "macrofiles: /usr/lib/rpm/macros: ... :${PWD}/rpmmacros"	> rpmrc
	rpmbuild --rcfile rpmrc -ba bbkeys.spec
