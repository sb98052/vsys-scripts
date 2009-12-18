#!/usr/bin/python -u
# All vsys scripts should use STDOUT in unbuferred mode, or else sometimes your output will get bufferred and you will not see it till the buffer gets flushed.  

import sys
import os

vsys_config_dir = "/etc/planetlab/vsys-attributes"

slicename=sys.argv[1]
sliceid = pwd.getpwnam(slicename).pw_uid

arglines = map(string.strip, sys.stdin.readlines())
command_name = arglines[0]
device_names = arglines[1:]

print "Validating interface names... ",
# Validate interface names

for vif in device_names:
    if len(vif)>16:
        print "Interface name %s invalid"%(vif)
        sys.exit(1)
    if re.match(r'(tun|tap)%d-\d+' % sliceid, vif ) is None:
        print "Interface name %s does not match slice id %d."%(vif, sliceid)
        sys.exit(1)
    print "[OK]"

# The interfaces have been validated. We don't need to validate the executable
# path for escape hatches because we are going to use execve.

pid = os.fork()
if (pid):
    # Close open fds before execve
    print "Closing file descriptors."
    for fd in xrange(3, 1023):
        try:
            os.close(fd)
        except OSError:
            pass
    # Execute command
    vserver_command = "/usr/sbin/vserver"
    args = [slicename]
    args += ['exec']
    args += [command_name]

    os.system('touch /etc/vservers/%s/spaces/net'%slicename)

    try:    
        os.execve(vserver_command, args)
    except:
        pass

    os.system('rm /etc/vservers/%s/spaces/net'%slicename)
else:
    for vif in device_names:
        os.system('/sbin/ip link set %s netns %d'%(vif, pid))

