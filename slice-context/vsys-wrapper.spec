#
# Vsys wrapper for slice space
#
# RPM spec file
#

%define name vsys-wrapper
%define version 0.95
%define taglevel 43

%define release %{taglevel}%{?pldistro:.%{pldistro}}%{?date:.%{date}}

Vendor: PlanetLab
Packager: PlanetLab Central <support@planet-lab.org>
Distribution: PlanetLab %{plrelease}
URL: %{SCMURL}

Summary: Vsys Wrapper
Name: %{name}
Version: %{version}
Release: %{release}
License: GPL
Group: System Environment/Kernel
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot

Source0: vsys-wrapper-%{version}.tar.gz

%description
Vsys wrapper provides convenience tools for the slice space, so users can more easily leverage vsys-scripts through python wrappers.

%prep 
%setup

%build
rm -rf $RPM_BUILD_ROOT
make -C slice-context

%install
mkdir -p $RPM_BUILD_ROOT/vsys
make -C slice-context install DESTDIR="$RPM_BUILD_ROOT"

%clean
rm -rf $RPM_BUILD_ROOT

%files
/usr/lib*/python*/site-packages

%post

%postun

%changelog
