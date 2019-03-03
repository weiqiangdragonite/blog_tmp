#!/bin/bash
#
# Program:
#   Use id, finger command to check system account's information.
#
# History:
#   2009/02/18  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

# get the user's account
users=$(cut -d ':' -f 1 /etc/passwd)

# check
for username in $users
do
    echo "----------"
    id $username
done

exit 0
