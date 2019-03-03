#!/bin/bash
#
# Program:
#   Use ping command to check the network's pc state
#
# History:
#   2009/02/18  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

network="192.168.2"
for ip in $(seq 1 200)
do
    #
    ping -c 1 -w 1 ${network}.${ip} &> /dev/null && result=0 || result=1
    #
    if [ "$result" == 0 ]; then
        echo "Server ${network}.${ip} is up"
    else
        echo "Server ${network}.${ip} is down"
    fi
done

exit 0
