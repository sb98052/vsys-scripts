#!/usr/bin/env python

# vsys.py: Python functions to wrap PlanetLab vsys API'
#   Alina Quereilhac - 02/09/2012
#
# Copyright (c) 202 INRIA
#

''' This extension provides easy to use Python functions to 
interact with the PlanetLab vsys API.

The vsys API is presented in the following publication:

Vsys: A programmable sudo
S Bhatia, G Di Stasi, T Haddow, A Bavier, S Muir, L Peterson
In USENIX 2011

'''

IFF_TUN = 0x0001
IFF_TAP = 0x0002

def vif_up(if_name, ip, prefix, snat = False):
    """ Configures a virtual interface with the values given by the user and 
    sets its state UP 

    Parameters:
     if_name:  the name of the virtual interface 
     ip:        the IP address to be assigned to the interface 
     prefix:    the network prefix associated to the IP address
     snat:      whether to enable SNAT on the virtual interface.

    Return value:
    On success, return 0. 
    On error, a RuntimeError is raised."""
    
    import _vsys
    (code, msg) = _vsys.vif_up(if_name, ip, str(prefix), snat)
    
    if code < 0:
        raise RuntimeError(msg)


def vif_down(if_name):
    """ Sets the state of a virtual interface DOWN 

    Parameters:
     if_name:  the name of the virtual interface 

    Return value:
    On success, return 0. 
    On error, a RuntimeError is raised."""
    
    import _vsys
    (code, msg) = _vsys.vif_down(if_name)
    
    if code < 0:
        raise RuntimeError(msg)

def fd_tuntap(if_type, no_pi = False):
    """Creates a TAP or TUN device in PlanetLab, and returns the device name and
    the associated file descriptor.
 
    Parameters:
        if_type:   the type of virtual device. Either IFF_TAP (0x0001) or
                    IFF_TUN (0x0002)
        no_pi:     set flag IFF_NO_PI

    Return value:
    On success, fd_tuntap returns a tuple containing the file descriptor 
    associated to the device and the device name assigned by the PlanetLab 
    vsys script.
    On error, a RuntimeError is raised."""

    import _vsys
    (fd, if_name) = _vsys.fd_tuntap(if_type, no_pi)

    if fd < 0:
        raise RuntimeError(if_name)

    return (fd, if_name)

def vroute(action, network, prefix, host, device):
    """ Adds or removes routes on PlanetLab virtual interfaces (TAP/TUN).

    Note that all networks and gateways must belong to the virtual 
    network segment associated to the vsys_vnet tag for the slice.

    Parameters:
        action:     either 'add' or 'del'
        network:    destintation network
        prefix:     destination network prefix
        host:       IP of gateway virtual interface
        device:     name of the gateway virtual interface

    Return value:
    On success, vroute returns 0.
    On error, a RuntimeError is raised."""

    import _vsys
    (code, msg) = _vsys.vroute(action, network, str(prefix), host, device)

    if code < 0:
        raise RuntimeError(msg)


