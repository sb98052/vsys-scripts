#
# Vsys filesystem
#
# RPM spec file
#
# $Id$
#

%define name vsys-scripts
%define version 0.95
%define taglevel 8

%define release %{taglevel}%{?pldistro:.%{pldistro}}%{?date:.%{date}}

Vendor: PlanetLab
Packager: PlanetLab Central <support@planet-lab.org>
Distribution: PlanetLab %{plrelease}
URL: %(echo %{url} | cut -d ' ' -f 2)

Summary: Vsys scripts scripts
Name: %{name}
Version: %{version}
Release: %{release}
License: GPL
Group: System Environment/Kernel
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
Requires: vsys

Source0: vsys-scripts-%{version}.tar.gz

%description
Vsys scripts for privileged operations on PlanetLab. These scripts are defined by maintainers of various components,
to which users require privileged access.

%prep
%setup

%build
rm -rf $RPM_BUILD_ROOT
make

%install
mkdir -p $RPM_BUILD_ROOT/vsys
cp exec/* $RPM_BUILD_ROOT/vsys

%clean
rm -rf $RPM_BUILD_ROOT

%files
/vsys/*

%post

%postun

%changelog
* Wed Sep 09 2009 Faiyaz Ahmed <faiyaza@cs.princeton.edu> - vsys-scripts-0.95-8
- Added script to create UDP sockets instead of TCP sockets w/ big buffers

* Mon Sep 07 2009 Thierry Parmentelat <thierry.parmentelat@sophia.inria.fr> - vsys-scripts-0.95-7
- minor changes in sliceip and ipfw, new reserve_udp_port

* Mon Aug 03 2009 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-6
- Added reserve_tcp_port and Giovanni's slice_ip tool.

* Tue Jul 07 2009 Thierry Parmentelat <thierry.parmentelat@sophia.inria.fr> - vsys-scripts-0.95-5
- tweak in ipfw-be for in-node emulation

* Thu Jun 11 2009 Thierry Parmentelat <thierry.parmentelat@sophia.inria.fr> - vsys-scripts-0.95-4
- Added the ipfw backend script

* Fri Jun 05 2009 Faiyaz Ahmed <faiyaza@cs.princeton.edu> - vsys-scripts-0.95-3
- rsync authlogs to ~/

* Mon May 18 2009 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-2
- Security update. umount had an issue that might have been exploited.

* Thu May 14 2009 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-1
- Adding port reservation script (goodfences)
- Modifying umount to be more (or less, depending on your point of view) robust



%define module_current_branch 0.7
