#!/bin/bash
#
# Should be root
#


# Get the IP and MAC address on this machine
local_ip=$(ifconfig enp0s3 | grep 'inet' | sed 's/^.*inet //g' | \
	sed 's/  netmask.*$//g' | sed 's/.*inet6.*$//g')
local_mac=$(ifconfig enp0s3 | grep 'ether' | sed 's/^.*ether //g' | \
	sed 's/  txqueuelen.*$//g')

echo "local_ip = ${local_ip}"
echo "local_mac = ${local_mac}"

file_name="addr.txt"

echo "IP = ${local_ip};  MAC = ${local_mac} (Local Machine)" > ${file_name}
echo "" >> ${file_name}


echo "Start clean ARP cache ..."
for ((net_id=1; net_id<255; ++net_id))
do
	ip_address="10.192.131.${net_id}"

	arp -d $ip_address > /dev/null
done
echo "Clean ARP cache done."


echo "Start catching network IP and MAC address ..."
nmap -sP 10.192.131.0/24 > /tmp/nmap.txt


# Use Regular Expression to get the IP and MAC address from the output of nmap
ip=""
mac=""
index=0
cat /tmp/nmap.txt | while read line
do
	string=$(echo $line | grep "report")
	if [ "$string" != "" ]; then
		ip=$(echo $string | sed 's/^.*for //g')
		index=1
	fi

	string=$(echo $line | grep "MAC")
	if  [ "$string" != "" ]; then
		mac=$(echo $string | sed 's/^.*Address: //g' | \
			sed 's/ (.*$//g')
		index=2
	fi

	if [ $index == 2 ]; then
		echo "IP = ${ip};  MAC = ${mac}" >> ${file_name}
		index=0
	fi
done

echo "Get IP and MAC address Done."
echo ""
exit 0
