#!/bin/bash
#
# Program:
#   This program shows the user's choice
#
# History:
#   2005/08/25  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

read -p "please input (Y/N): " yn

#
[ "$yn" == "Y" -o "$yn" == "y" ] && echo "OK, continue" && exit 0
[ "$yn" == "N" -o "$yn" == "n" ] && echo "Oh, interrupt!" && exit 0

#
echo "I don't know your choice is" && exit 0
