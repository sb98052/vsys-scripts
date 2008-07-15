#
# Vsys filesystem
#
# RPM spec file
#
# $Id: vsys-factory.spec 9786 2008-07-02 08:54:09Z thierry $
#

%define name vsys
%define version 0.8
%define taglevel 16

%define release %{taglevel}%{?pldistro:.%{pldistro}}%{?date:.%{date}}

Vendor: PlanetLab
Packager: PlanetLab Central <support@planet-lab.org>
Distribution: PlanetLab %{plrelease}
URL: %(echo %{url} | cut -d ' ' -f 2)

Summary: Vsys factory scripts
Name: %{name}
Version: %{version}
Release: %{release}
License: GPL
Group: System Environment/Kernel
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
Requires: vsys

Source0: vsys-factory-%{version}.tar.gz

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
cp * $RPM_BUILD_ROOT/vsys

%clean
rm -rf $RPM_BUILD_ROOT

%files
/vsys/*

%post
if [ "$PL_BOOTCD" != "1" ] ; then
        service vsys restart
fi

%postun

%changelog

