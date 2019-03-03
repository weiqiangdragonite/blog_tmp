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
    echo -n "You choice is "
}

echo "This program will print your selection!"

#read -p "Input your choice: " choice
#case $choice in

case $1 in
    "one")
        printit; echo $1 | tr 'a-z' 'A-Z'
        ;;
    "two")
        printit; echo $1 | tr 'a-z' 'A-Z'
        ;;
    "three")
        printit; echo $1 | tr 'a-z' 'A-Z'
        ;;
    *)
        echo "Usage $0 {one|two|three}"
        ;;
esac

exit 0
