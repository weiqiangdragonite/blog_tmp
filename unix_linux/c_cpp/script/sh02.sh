#!/bin/bash
#
# Program:
#   This program read the user's first name and last name, then print the
#   full name on the screen.
#
# Author:
#   Dragonite
#
# Date:
#   2013-7-4

# setting the path
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

# read the user's first name
echo "Please input your first name"
read firstName

# read the user's last name
read -p "Please input your last name: " lastName

# output the full name
echo "Your full name is: " $firstName $lastName

exit 0
