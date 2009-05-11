#
# Vsys filesystem
#
# RPM spec file
#
# $Id: vsys-scripts.spec 9786 2008-07-02 08:54:09Z thierry $
#

%define name vsys-scripts
%define version 0.9
%define taglevel 3

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
* Sun May 10 2009 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.9-3
- Removed a check from umount for users to be able to unmount directories they managed to mess up with FUSE

* Wed Apr 08 2009 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.9-2
- Including the umount script needed to unmount fuse mounts

* Mon Apr 06 2009 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.9-1
- Tagging to check the sanity of the taglevel, branch number etc.



%define module_current_branch 0.7
