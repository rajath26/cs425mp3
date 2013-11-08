#!/bin/bash

############################################################################
# This is a start up script for The Daisy Distributed System Failure Detector 
###########################################################################

#
#
# USAGE: ./startDaisyFDNode.sh -p <udp_port_no> -q <tcp_port_no> -i <ip_address_of_the_current_server> -t <node_type> -h <host_no>
#
#

if [ $# -ne 8 ]
then 
    echo -e "INVALID number of arguments"
    echo -e "USAGE: $0 -p <udp_port_no> -q <tcp_port_no> -i <ip_address_of_the_current_server> -t <node_type> -h <host_no>"
    exit 1
fi

while getopts p:i:t:h: opt
do
    case $opt in
        p)
        # UDP PORT NO
        UDP_PORT_NO=$OPTARG;;
        q)
        # TCP PORT NO
        TCP_PORT_NO=$OPTARG;;
        i)
        # IP ADDRESS
        IP_ADDRESS=$OPTARG;;
        t)
        # NODE TYPE
        TYPE=$OPTARG;;
        h)
        # HOST NO
        HOST_NO=$OPTARG;;
     esac
done

if [ $? -eq 0 ]
then 
    echo -e "\nStarting Node\n"
    ./daisyFD ${UDP_PORT_NO} ${TCP_PORT_NO} ${IP_ADDRESS} ${TYPE} ${HOST_NO}
fi

#
# End of script
#
