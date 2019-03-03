#!/bin/bash
#
# Program:
#   This script only accepts the following parameter: one, two, three.
# History:
#   2005/08/29  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

echo "This program will print your selection!"

read -p "Input your choice: " choice
case $choice in

#case $1 in
    "one")
        echo "Your choice is ONE"
        ;;
    "two")
        echo "Your choice is TWO"
        ;;
    "three")
        echo "Your choice is THREE"
        ;;
    *)
        echo "Usage $0 {one|two|three}"
        ;;
esac

exit 0
