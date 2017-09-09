# doc/Makefile.am for bbkeys

EXTRA_DIST = \
	bbkeys.pod \
	bbkeysrc.pod

bbkeys_man1_files = \
	bbkeys.man

dist_man1_MANS = $(bbkeys_man1_files)

$(bbkeys_man1_files): %.man: %.pod
	$(AM_V_GEN)pod2man --center="User Commands" \
		--date="$(DATE)" \
		--errors="none" \
		--fixed="CW" \
		--fixedbold="CB" \
		--fixeditalic="CI" \
		--fixedbolditalic="CX" \
		--nourls \
		--release="$(PACKAGE) $(VERSION)" \
		--section="1" \
		$< >$@

bbkeys_man5_files = \
	bbkeysrc.man

dist_man5_MANS = $(bbkeys_man5_files)

$(bbkeys_man5_files): %.man: %.pod
	$(AM_V_GEN)pod2man --center="Standards, Environments and Macros" \
		--date="$(DATE)" \
		--errors="none" \
		--fixed="CW" \
		--fixedbold="CB" \
		--fixeditalic="CI" \
		--fixedbolditalic="CX" \
		--nourls \
		--release="$(PACKAGE) $(VERSION)" \
		--section="5" \
		$< >$@

