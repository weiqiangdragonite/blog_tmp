#!/bin/bash
#
# Program:
#   This script use whoami and pwd command shows the info.
#
# By:
#   Dragonite
#   2013-7-21

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

echo "You are $(whoami)"
echo "You are under the '$(pwd)' directory"

exit 0
