Summary: bbkeys, a completely configurable key-combo grabber for blackbox
Name: bbkeys
Version: 0.8.1
Release: 1
Copyright: GPL
Group: X11/Applications
Source: http://movingparts.net/bbkeys/bbkeys-%{version}.tar.gz
URL: http://movingparts.net
Packager: vanRijn (vR@movingparts.net)
prefix: /usr/local
Buildroot: /var//tmp/%{name}-buildroot

%description
bbkeys is a configurable key-grabber designed for the blackbox window manager
which is written by Brad Hughes.  It is based on the bbtools object code
created by John Kennis and re-uses some of the blackbox window manager classes
as well.  bbkeys is easily configurable via directly hand-editting the user's
~/.bbkeysrc file, or by using the provided gui configuration tool,
bbkeysconf (for lack of a better name yet).  
%prep
%setup
./configure --prefix=%{prefix}

%build
make

%install
rm -rf $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT/%{prefix} install


%clean
rm -rf $RPM_BUILD_ROOT

%files 
%defattr(-,root,root) 
%{prefix}/bin/bbkeys 
%{prefix}/bin/bbkeysConfigC 
%{prefix}/share/bbtools/README.bbkeys 
%{prefix}/share/bbtools/bbkeys.bb 
%{prefix}/share/bbtools/bbkeys.nobb 
%{prefix}/doc/bbkeys/README
%{prefix}/doc/bbkeys/AUTHORS
%{prefix}/doc/bbkeys/ChangeLog
 
%changelog 
* Sun Aug 5 2001 Jason Kasper <vR@movingparts.net>
- added to file list for new included files
- install to %{prefix} instead of /usr
* Sun May 6 2001 Hollis Blanchard <hollis@terraplex.com> 
- removed file list in favor of explicit %files section 
- install to /usr instead of /usr/local 
- buildroot = /var/tmp/bbkeys-buildroot instead of /tmp/buildroot
