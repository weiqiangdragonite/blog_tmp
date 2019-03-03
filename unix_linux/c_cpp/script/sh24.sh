#!/bin/bash
#
# Program:
#   This script
#
# By:
#   Dragonite
#   2013-7-21

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

function print() {
    echo "The $1 account is \"$2\""
}

i=1
userlist=$(cut -d ':' -f 1 /etc/passwd)
for username in $userlist
do
   print $i $username
   let ++i
done
       
exit 0
