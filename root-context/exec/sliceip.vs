#!/bin/bash
#
# Giovanni Di Stasi
# Copyright (C) 2009 UniNa
#
# This is a backend script to be used with
# the vsys system. It allows to define and 
# manage slice-specific routing tables. 
# The routing rules added to these routing 
# tables affect only the respective slices.  
#
# This script is meant to be called from a 
# frontend script in the slice environment. 
#
# Fronted usage:
# sliceip enable <interface> 
#   This is the first command to issue; it enables the use
#   of the interface for the slice. This command has to be issued 
#   for each interface involved in the slice-specific routing rules.
#
# sliceip disable <interface> 
#   This is to disable the use of the interface for the slice.
#
# sliceip route show 
#   This command shows the current routing rules of the
#   slice-specific routing table
#
# sliceip route add to <destination> via <gateway> dev <interface>
#   This command adds a rule to reach <destination> through the host
#   <gateway> that can be reached on the interface <interface>
# ...
# 
# sliceip in general supports all the commands of the "ip" command of
# the "iproute2" suite. Refer to ip manpage for further information.
# 
# Some examples:
#	sliceip route add to 143.225.229.142 via 192.168.8.2 dev ath0
#	sliceip route add default via 10.1.1.1 dev ath0
#
# If you want to test the script from the root context you need 
# to call it as vsys would do:
# sliceip <slice_name>
# Then you can issue to its standard input the commands previously explained
# (but you have to remove the initial "sliceip"). Ex.:
#	 route add to 143.225.229.142 via 192.168.8.2 dev ath0


PATH=/bin:/usr/bin:/sbin:/usr/sbin 

# sliver wich called the script
sliver=$1

#files used
RT_TABLES="/etc/iproute2/rt_tables"
INT_FILE="/tmp/slcroute_ifns"

# routing tables from 100 to 255 are used
FIRST_TABLE=100

DEBUG=0

#this script can also work on a Linux machine, in wich case
#it enables user-specific routing tables
LINUX_ENV=0 # 0 - Linux host;
	    # does not work for the icmp protocol

PLANET_ENV=1 # 1 - PlanetLab context (slice-specific routing) 

#select the PlanetLab environment
ENVIRONMENT=$PLANET_ENV


# enable an interface for the slice specific routing rules
# and initialise the slice routing table
function enable_interface(){ # $1 calling sliver; $2 interface to enable
	local sliver=$1
	local interface="$2"
	local ip=`get_address $interface`

	if [[ "" == $ip ]]; then
		echo "Failed in getting address of $interface"
		return 1
	fi

	#create the sliceip netfilter chains
	check_chains

	local nid=`get_nid $sliver`
	local num_active=`get_num_activated $sliver`

	if ! is_active $sliver $interface; then
		if [[ $num_active == 0 ]]; then
				#define the routing table for the slice and set some iptables rules in order to mark user's packets;			
				create_table $sliver
		fi

		#Adds an SNAT rule to set the source IP address of packets that are about to go out through $interface.
		#The kernel sets the source address of packets when the first routing process happens (following the rules 
		#of the main routing table); when the rerouting process happens (following the user/slice specific rules) 
	  	#the source address is not modified. Consequently, we need to add this rule to change the source ip address 
		#to the $interface source address.
	 	exec "iptables -t nat -A $NAT_POSTROUTING -o $interface -j SNAT --to-source $ip -m mark --mark $nid"

		set_active $sliver $interface $ip		
	else
		local old_ip=`get_ip_from_file $sliver $interface`
	
		#if the ip of the interface has changed, set the new SNAT rule
		if [[ $old_ip != $ip ]]; then
			#remove the rule for the old ip
			exec "iptables -t nat -D $NAT_POSTROUTING -o $interface -j SNAT --to-source $old_ip -m mark --mark $nid"
			#insert the new rule			
			exec "iptables -t nat -A $NAT_POSTROUTING -o $interface -j SNAT --to-source $ip -m mark --mark $nid"	
			set_deactivated $sliver $interface $old_ip
			set_active $sliver $interface $ip
		fi


	fi

}

# disable the interface for the slice-specific routing rules
function disable_interface(){
	local sliver=$1
	local interface=$2
	local ip=`get_ip_from_file $sliver $interface`	
	local nid=`get_nid $sliver`

	if is_active $sliver $interface >/dev/null 2>&1; then
		exec "iptables -t nat -D $NAT_POSTROUTING -o $interface -j SNAT --to-source $ip -m mark --mark $nid"
		
		local num_ifn_on=`get_num_activated $sliver`
		
		if [[ $num_ifn_on == 1 ]]; then
			delete_table $sliver
		fi
		set_deactivated $sliver $interface
	fi

}

# wrapper function used to execute system commands
function exec(){
	local command=$1	

	if ! [[ $command ]]; then
		echo "Error in exec. No argument."
		exit 1	
	else
		if ! $command; then
			if [[ $DEBUG == 1 ]]; then echo "Error executing \"$1\""; fi
			exit 1
		fi
	fi

}

# decides wich id to use for the slice-specific routing table
function get_table_id(){
	local sliver=$1
	k=$FIRST_TABLE

	while [[ $k < 255 ]] && grep $k $RT_TABLES >/dev/null 2>&1 ; do
		k=$((k+1))
	done

	if [[ $k == 255 ]]; then
		logm "No routing tables available. Exiting."
		return 1
	fi
	
	echo $k
}


# create the slice-specifig routing table
function create_table(){
	local sliver=$1
	local table_name=`get_table_name $sliver`
	local table_id=`get_table_id $sliver`	
	local temp_nid=`get_temp_nid $sliver`

	if ! grep $table_name $RT_TABLES > /dev/null 2>&1; then
		echo "$table_id $table_name" >> $RT_TABLES
  	else
		echo "WARNING: $table_name routing table already defined."
	fi

	set_routes $sliver $table_name

	return 0
}

# delete the slice-specific routing table
function delete_table(){
	local sliver=$1
	local table_name=`get_table_name $sliver`
	local table_id=`get_nid $sliver`	
	local temp_nid=`get_temp_nid $sliver`

	if ! grep $table_name $RT_TABLES > /dev/null 2>&1; then
		return 1			
	else
		exec "ip route flush table $table_name" >/dev/null 2>&1
		unset_routes $sliver $table_name
		remove_line "$RT_TABLES" "$table_name"  	
	fi

	return 0
}

# remove a line from a file
function remove_line(){
	local filename=$1
	local regex=$2

	#remove interface line from the file
	exec "sed -i /${regex}/d $filename"
}

# get the slice-specific routing table name
function get_table_name(){
	local sliver=$1;
	echo "${sliver}_slcip"
}

#remove files used by sliceip and the added routing tables 
function clean_iproute_conf(){
	while grep "slcip" $RT_TABLES >/dev/null 2>&1; do
		remove_line $RT_TABLES "slcip"
	done

	rm -f ${INT_FILE}-*

}

#get slice network id
function get_nid(){
    id -u ${1}
}

# set the firewall rules. Mainly, it asks VNET+ to set the netfilter mark of packets belonging to the slice to the slice-id
# and then associates those packets with the routing table allocated for that slice.
function set_routes(){
	local sliver=$1
	local table_name=$2
	local sliver_nid=`get_nid $sliver`
	local temp_nid=`get_temp_nid $sliver`

	if [ $ENVIRONMENT == $PLANET_ENV ]; then	
		#Linux kernel triggers a rerouting process, wich is needed to perfom slice-specific routing,  
		#if it sees that the netfilter mark has been altered in the iptables mangle chain.
		#As VNET+ sets the netfilter mark of some packets in advance (to the slice-id value) before they enter the iptables mangle chain, 
		#we need to change it here (otherwise the rerouting process is not triggered for them).
		exec "iptables -t mangle -I $MANGLE_OUTPUT 1 -m mark --mark $sliver_nid -j MARK --set-mark $temp_nid"

		#make sure the netfilter mark of those "strange packets" won't be changed by the following rule
		exec "iptables -t mangle -I $MANGLE_OUTPUT 2 -m mark --mark $temp_nid -j RETURN"

		#Here we ask VNET+ to set the netfilter mark for the remaining packets (to the slice-id) 
		#we need to call this only once
		if [[ $first_time == 1 ]]; then
			exec "iptables -t mangle -A $MANGLE_OUTPUT -j MARK --copy-xid 0x00"
		fi		

	elif [ $ENVIRONMENT == $LINUX_ENV ]; then
		#the same in the case of a "plain" Linux box. In this case we do not use VNET+ but
		#the owner module of iptables.
		exec "iptables -t mangle -A $MANGLE_OUTPUT -m owner --uid-owner $sliver_nid -j MARK --set-mark $sliver_nid"
	fi

	if [ $ENVIRONMENT == $PLANET_ENV ]; then	
		#Here the netfilter mark is restored to the slice-id value for the "strange packets"
		exec "iptables -t mangle -A $MANGLE_POSTROUTING -m mark --mark $temp_nid -j MARK --set-mark $sliver_nid"
	fi

	#Set the routing for the slice to be applied following the rules in $table_name"
	#for the slice packets...
  	exec "ip rule add fwmark $sliver_nid table $table_name"	
	#...and for the "strange packets"  	
	exec "ip rule add fwmark $temp_nid table $table_name"	

	exec "ip route flush cache"  >/dev/null 2>&1
}


# remove the firewall rules. 
function unset_routes(){
	local sliver=$1
	local table_name=$2
	local sliver_nid=`get_nid $sliver`
	local temp_nid=`get_temp_nid $sliver`
	
	if [ $ENVIRONMENT == $PLANET_ENV ]; then	 
		#removes the rules for the netfilter marks (for the "strange packets")
		exec "iptables -t mangle -D $MANGLE_OUTPUT -m mark --mark $sliver_nid -j MARK --set-mark $temp_nid"
		exec "iptables -t mangle -D $MANGLE_OUTPUT -m mark --mark $temp_nid -j RETURN"
		
		#removes the rule for restoring the original mark
		exec "iptables -t mangle -D $MANGLE_POSTROUTING -m mark --mark $temp_nid -j MARK --set-mark $sliver_nid"

	elif [ $ENVIRONMENT == $LINUX_ENV ]; then
		#removes the rules that asks the owner module of iptables to set the netfilter mark to the user-id
		exec "iptables -t mangle -D $MANGLE_OUTPUT -m owner --uid-owner $sliver_nid -j MARK --set-mark $sliver_nid"
	fi

  	exec "ip rule del fwmark $temp_nid table $table_name"
	exec "ip rule del fwmark $sliver_nid table $table_name"	

	exec "ip route flush cache"

}

# additional iptables chains where sliceip inserts its rules
NAT_POSTROUTING="sliceip"
MANGLE_OUTPUT="sliceip_output"
MANGLE_POSTROUTING="sliceip_postrouting"

# create the chains
check_chains(){
	first_time=0

	#create the chain where SNAT is performed
	if iptables -t nat -N $NAT_POSTROUTING >/dev/null 2>&1; then
		#it's the first time sliceip is called, the chain was not defined		
		first_time=1		
		
		#create a chain where the netfilter mark is set
		exec "iptables -t mangle -N $MANGLE_OUTPUT"

		#create a chain where the netfilter mark for some packets is restored (see set_routes)
		exec "iptables -t mangle -N $MANGLE_POSTROUTING"

		#add the rules to take packets to the previously defined chains
		exec "iptables -t nat -A POSTROUTING -j $NAT_POSTROUTING"		
		exec "iptables -t mangle -A OUTPUT -j $MANGLE_OUTPUT"
		exec "iptables -t mangle -I POSTROUTING 1 -j $MANGLE_POSTROUTING"

		#cleaning up
		clean_iproute_conf
	fi
}

# get the ip address of an interface
function get_address(){
	local interface=$1	
	local ip=""
	
	ip=`ifconfig $interface | grep inet\ addr | cut -d ":" -f 2 | cut -d " " -f 1`;

	if valid_dotted_quad $ip; then 
		echo $ip;
	else 
		echo "";
	fi	
			
}

# get the temporary mark to be applied to the packets of the slice
function get_temp_nid(){
	local sliver_nid=`get_nid $1`
	local temp_nid=$((0x20000+$sliver_nid))
	echo $temp_nid
}

# log function
function logm(){
	logger $1
}

# get the name of the filename in wich we store information about
# the interfaces in use by the user
function get_filename_sliver(){
	local sliver=$1
	echo "${INT_FILE}-$sliver"
}

function set_active(){
	local sliver=$1
	local interface=$2
	local ip=$3
  	local filename="${INT_FILE}-$sliver"
	echo "$interface $ip" >> $filename
}

function set_deactivated(){
	local sliver=$1
	local interface=$2
  	local filename=`get_filename_sliver $sliver`
	remove_line $filename $interface
}

function is_active(){
	local sliver=$1
	local interface=$2
  	local filename=`get_filename_sliver $sliver`

	if grep $interface $filename >/dev/null 2>&1; then
		return 0
	else
		return 1
	fi
}

function get_num_activated(){
	local sliver=$1
	local filename=`get_filename_sliver $sliver`

	if ! [ -e $filename ]; then
		echo 0;
	else
		wc -l $filename | cut -f 1 -d " ";
	fi
}


function get_ip_from_file(){
	local sliver=$1
	local interface=$2

	local filename=`get_filename_sliver $sliver`

	cat $filename | grep $interface | cut -d " " -f 2
}


# check ip addresses  
function valid_dotted_quad(){
    oldIFS=$IFS
    IFS=.
    set -f
    set -- $1
    if [ $# -eq 4 ]
    then
      for seg
      do
        case $seg in
            ""|*[!0-9]*) return 1; break ;; ## Segment empty or non-numeric char
            *) [ $seg -gt 255 ] && return 2 ;;
        esac
      done
    else
      return 3 ## Not 4 segments
    fi
    IFS=$oldIFS
    set +f
    return 0;
}


# BEGIN
# the script starts here

if [[ $sliver == "" ]]; then
	echo "I need the first argument (the sliver name)";
	exit 1
fi

# read a line from the vsys pipe
read line

# separate the first word of the line from the others
command=`echo ${line%% *}`
rest=`echo ${line#* }`

case "$command" in
  enable)
	logger "sliceip command received from $sliver: $line"	
	enable_interface $sliver "$rest"
	;;

  disable)
	logger "sliceip command received from $sliver: $line"	
	disable_interface $sliver "$rest"
	;;

  *)
	logger "sliceip command received from $sliver: $line"
  	table_sliver=`get_table_name $sliver`

	if ! grep "$table_sliver" $RT_TABLES >/dev/null 2>&1; then
		echo "Error. The slice routing table is not defined. Execute sliceip enable <interface>."
		exit 1
	else 
		#add the routing rule - ip is called with the same parameters of sliceip but the indication of 
		#the table in wich the rule is to be inserted is appended		
		exec "ip $line table $table_sliver"
	fi  
	;;

esac

exit 0
