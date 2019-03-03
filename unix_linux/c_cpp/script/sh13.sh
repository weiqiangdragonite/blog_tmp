#!/bin/bash
#
# Program:
#   Repeat question until user input correct answer.
#
# History:
#   2005/08/29  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

while [ "$yn" != "yes" -a "$yn" != "YES" ]
do
    read -p "Please input yes/YES to stop this program: " yn
done

echo "OK! You input correct answer."

exit 0
