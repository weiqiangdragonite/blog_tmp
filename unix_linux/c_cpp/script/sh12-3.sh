#!/bin/bash
#
# Program:
#   Use function to repeat information.
#
# History:
#   2005/08/29  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH


# function
function printit() {
    echo "You choice is $1"
}

echo "This program will print your selection!"

#read -p "Input your choice: " choice
#case $choice in

case $1 in
    "one")
        printit 1
        ;;
    "two")
        printit 2
        ;;
    "three")
        printit 3
        ;;
    *)
        echo "Usage $0 {one|two|three}"
        ;;
esac

exit 0
