Summary: bbkeys, a completely configurable key-combo grabber for blackbox
Name: bbkeys
Version: 0.3.6
Release: 1
Copyright: GPL
Group: X11/Applications
Source: http://movingparts.net/bbkeys/bbkeys-0.3.6.tar.gz
URL: http://movingparts.net
Packager: vanRijn (vR@movingparts.net)
Buildroot: /tmp/buildroot

%description
bbkeys is a configurable key-grabber designed for the blackbox window manager
which is written by Brad Hughes.  It is based on the bbtools object code
created by John Kennis and re-uses some of the blackbox window manager classes
as well.  bbkeys is easily configurable via directly hand-editting the user's
~/.bbkeysrc file, or by using the provided gui configuration tool,
bbkeysconf (for lack of a better name yet).  
%prep
%setup
./configure

%build
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT

make prefix=$RPM_BUILD_ROOT/usr/local install
cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/file.list.qt
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.qt
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.qt


%clean
rm -rf $RPM_BUILD_ROOT
rm -rf ../file.list.qt

%files -f ../file.list.qt
