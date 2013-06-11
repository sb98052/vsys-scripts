#
# Vsys filesystem
#
# RPM spec file
#

%define name vsys-scripts
%define version 0.95
%define taglevel 47

%define release %{taglevel}%{?pldistro:.%{pldistro}}%{?date:.%{date}}

Vendor: PlanetLab
Packager: PlanetLab Central <support@planet-lab.org>
Distribution: PlanetLab %{plrelease}
URL: %{SCMURL}

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
make -C root-context

%install
mkdir -p $RPM_BUILD_ROOT/vsys
cp root-context/exec/* $RPM_BUILD_ROOT/vsys

%clean
rm -rf $RPM_BUILD_ROOT

%files
/vsys/*

%post

%postun

%changelog
* Tue Jun 11 2013 Thierry Parmentelat <thierry.parmentelat@sophia.inria.fr> - vsys-scripts-0.95-47
- Giuseppe Lettieri's patch for ignoring glitches when tearing down iptables rules

* Tue Apr 23 2013 Thierry Parmentelat <thierry.parmentelat@sophia.inria.fr> - vsys-scripts-0.95-46
- vif_up can set dropkern and mtu

* Wed Dec 19 2012 Thierry Parmentelat <thierry.parmentelat@sophia.inria.fr> - vsys-scripts-0.95-45
- script 'promisc' can set a device in promiscuous mode

* Tue Sep 04 2012 Thierry Parmentelat <thierry.parmentelat@sophia.inria.fr> - vsys-scripts-0.95-44
- new slice-oriented package vsys-wrapper (python wrapper)
- new layout of the module, spit between root-context and slice-context

* Fri Jul 27 2012 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-43

* Mon Jul 09 2012 Thierry Parmentelat <thierry.parmentelat@sophia.inria.fr> - vsys-scripts-0.95-42
- fix TUN devices creation (was always creating TAPs)

* Tue Jun 12 2012 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-41

* Wed Jun 06 2012 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-40

* Tue May 08 2012 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-39
- prevent vserver from unsharing namespaces

* Thu Apr 19 2012 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-38
- This change resolves the pricky issue of bind mounting across namespaces by disabling namespaces altogether.

* Mon Apr 09 2012 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-37
- A new script that lets slices serve DNS by binding to UDP port 53.

* Tue Mar 20 2012 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-36
- Disable namespaces for pl_netflow

* Tue Feb 14 2012 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-35
- Added a script for users to create custom distributions within their slices. Use as follows:
- mkdir /rootfs;cd /rootfs
- wget <your distribution tarball>
- tar xvfz <tarball>
- [copy in PlanetLab /etc/resolv.conf and /etc/passwd files. mkdir /var/run; mkdir /var/lock]
- cat /vsys/switcheroo.out &
- echo 1 > /vsys/switcheroo.in

* Fri Jan 13 2012 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-34
- Added a new script from Jude.

* Mon Oct 24 2011 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-33
- Build fd_netlink, a new script that lets slices stream packet headers

* Mon Oct 03 2011 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-32
- New script for slices to stream the headers of all packets going out of a node.

* Wed Aug 31 2011 Thierry Parmentelat <thierry.parmentelat@sophia.inria.fr> - vsys-scripts-0.95-31
- vif_up has support for GRE tunnels
- sliceip is known to cause kernel crashes with 2.6.32 at least

* Mon Jul 18 2011 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-30
- New script: claimport.

* Tue Jun 07 2011 Thierry Parmentelat <thierry.parmentelat@sophia.inria.fr> - vsys-scripts-0.95-29
- vif_down,vif_up: more tidy, pointopoint, and work around ip route get <> oddly returning nothing
- new vroute

* Mon Feb 21 2011 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-28
- Bug fix for 2.6.27+ kernels

* Wed Feb 02 2011 Thierry Parmentelat <thierry.parmentelat@sophia.inria.fr> - vsys-scripts-0.95-27
- first draft of new scripts to handle openvswitch

* Tue Jan 11 2011 Andy Bavier <acb@cs.princeton.edu> - vsys-scripts-0.95-26
- rsyncnodelogs script, make it easy to bring up tun/tap devices

* Mon Oct 18 2010 S.Çağlar Onur <caglar@cs.princeton.edu> - vsys-scripts-0.95-25
- support newer kernels

* Fri Oct 15 2010 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-24
- Add executable tag to vif_up to let it run.

* Fri Oct 15 2010 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-23
- Added execute attribute to vif_up, the absence of which prevented it from being used.

* Mon Oct 11 2010 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-22
- vif_up has now been modified with the ability to create tap devices and to be able to SNAT over randomly selected
- ports. Thanks to Matthias Goerner for his help developing and testing these changes.

* Fri Sep 17 2010 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-21
- Added script fd_packetseer, also updated tun/tap script to be able to create tun devices in addition to tap devices
- and other minor changes.

* Mon Jul 19 2010 Baris Metin <Talip-Baris.Metin@sophia.inria.fr> - vsys-scripts-0.95-20
- fix dotsshmount

* Thu Jul 01 2010 Baris Metin <Talip-Baris.Metin@sophia.inria.fr> - vsys-scripts-0.95-19
- change dotsshmount's mount point

* Fri Jun 25 2010 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-18
- Switch fd_tuntap to 2.6.27.

* Fri Apr 30 2010 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-17
- Tweak to if_port_forward script.

* Fri Apr 30 2010 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-16
- Added if_port_forward script to enable slices to forward ports for their
- allocated IP addresses.

* Wed Mar 03 2010 Talip Baris Metin <Talip-Baris.Metin@sophia.inria.fr> - vsys-scripts-0.95-15
- - hostname filter and updated documentation for ipfw scripts - Marta
- - vsys script for letting users to set the Type of Services(ToS) byte in the IPv4 header - Caglar
- (if this is not needed anymore please remove and tag again)

* Thu Jan 21 2010 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-14
- Tweak to Thom Haddow's script to let a slice see packets related to
- existing connections.

* Mon Jan 11 2010 Thierry Parmentelat <thierry.parmentelat@sophia.inria.fr> - vsys-scripts-0.95-13
- ipfw frontend has a rules cleanup feature
- ongoing work around vif_up
- new script 'run_with_devices'

* Thu Nov 26 2009 Thierry Parmentelat <thierry.parmentelat@sophia.inria.fr> - vsys-scripts-0.95-12
- improve usability of the ipfw backend

* Wed Oct 28 2009 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-11
- * Load the tun module if it is not already loaded
- * Fixed a bug in fd_tuntap

* Tue Oct 27 2009 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-10
- Adds Thom Haddow's tun/tap changes. I omitted a change in my previous commit.

* Tue Oct 27 2009 Sapan Bhatia <sapanb@cs.princeton.edu> - vsys-scripts-0.95-9
- This update adds Thom Haddow's custom tun/tap-device scripts.

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
