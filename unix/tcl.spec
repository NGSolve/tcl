# $Id: tcl.spec,v 1.24 2006/03/23 16:39:19 rmax Exp $
# This file is the basis for a binary Tcl RPM for Linux.

%{!?directory:%define directory /usr/local}

Name:          tcl
Summary:       Tcl scripting language development environment
Version:       8.5a4
Release:       2
License:       BSD
Group:         Development/Languages
Source:        http://prdownloads.sourceforge.net/tcl/tcl%{version}-src.tar.gz
URL:           http://www.tcl.tk/
Buildroot:     /var/tmp/%{name}%{version}

%description
The Tcl (Tool Command Language) provides a powerful platform for
creating integration applications that tie together diverse
applications, protocols, devices, and frameworks.  When paired with
the Tk toolkit, Tcl provides the fastest and most powerful way to
create GUI applications that run on PCs, Unix, and Mac OS X.  Tcl
can also be used for a variety of web-related tasks and for creating
powerful command languages for applications.

%prep
%setup -q -n %{name}%{version}

%build
cd unix
CFLAGS="%optflags" ./configure \
	--prefix=%{directory} \
	--exec-prefix=%{directory} \
	--libdir=%{directory}/%{_lib}
make 

%install
cd unix
make INSTALL_ROOT=%{buildroot} install

%clean
rm -rf %buildroot

%files
%defattr(-,root,root)
%if %{_lib} != lib
%{directory}/%{_lib}
%endif
%{directory}/lib
%{directory}/bin
%{directory}/include
%{directory}/man/man1
%{directory}/man/man3
%{directory}/man/mann
