#!/usr/bin/env python

#
# Copyright (c) 2012 INRIA
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
# Author: Alina Quereilhac <alina.quereilhac@inria.fr>
#

import subprocess
import unittest
import time

from vsys import fd_tuntap, vif_up, vif_down, vroute, IFF_TAP, IFF_TUN
import _vsys

class TestVsys(unittest.TestCase):
    def setUp(self):
        self._network = "192.168.2.0"
        self._prefix  = 30
        self._ip = "192.168.2.2"
        self._remote_net = "192.168.2.4"

    def _create_vif(self, if_type, no_pi = False):
        ####### create virtual device 
        (fd, if_name) = fd_tuntap(if_type, no_pi)
        self.assertTrue(fd > 0)
	
        ###### configure virtual device
        vif_up(if_name, self._ip, self._prefix)

	# wait for prcocess to see the new configuration...
	time.sleep(5)

	###### test ip responds to pings
        p = subprocess.Popen(["ping", "-qc3", self._ip], stdout=subprocess.PIPE, stdin=subprocess.PIPE)
        out, err = p.communicate()
        
        self.assertFalse(err)

        expected = """PING %(ip)s (%(ip)s) 56(84) bytes of data.

--- %(ip)s ping statistics ---
3 packets transmitted, 3 received, 0%% packet loss""" % {'ip': self._ip}
        
        self.assertTrue(out.startswith(expected), out)

	###### add route
        vroute ("add", self._remote_net, self._prefix, self._ip, if_name)

	# wait for prcocess to see the new configuration...
	time.sleep(5)

        ###### test routes
        p = subprocess.Popen(["ip", "r"], stdout=subprocess.PIPE, stdin=subprocess.PIPE)
        out, err = p.communicate()
        
        self.assertFalse(err)
	self.assertTrue(out.find(self._remote_net) >= 0 )

	###### del route
        vroute ("del", self._remote_net, self._prefix, self._ip, if_name)

	# wait for prcocess to see the new configuration...
	time.sleep(5)

        ##### test routes
        p = subprocess.Popen(["ip", "r"], stdout=subprocess.PIPE, stdin=subprocess.PIPE)
        out, err = p.communicate()
        
        self.assertFalse(err)
	self.assertTrue(out.find(self._remote_net) < 0 )

        ##### delete interface
        vif_down(if_name)

	# wait for prcocess to see the new configuration...
	time.sleep(5)

	###### test ip NOT responds to pings
        p = subprocess.Popen(["ping", "-qc3", self._ip], stdout=subprocess.PIPE, stdin=subprocess.PIPE)
        out, err = p.communicate()
        
        self.assertFalse(err)

        expected = """PING %(ip)s (%(ip)s) 56(84) bytes of data.

--- %(ip)s ping statistics ---
3 packets transmitted, 0 received, 100%% packet loss""" % {'ip': self._ip}
        
        self.assertTrue(out.startswith(expected), out)


    def test_create_tun(self):
        self._create_vif(IFF_TUN)

    def test_create_tap(self):
        self._create_vif(IFF_TAP)

    def test_create_tun_no_pi(self):
        self._create_vif(IFF_TUN, no_pi = True)

    def test_create_tap_no_pi(self):
        self._create_vif(IFF_TAP, no_pi = True)
 


if __name__ == '__main__':
    unittest.main()

