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
if [ "$yn" == "Y" ] || [ "$yn" == "y" ]; then
    echo "OK, continue"
elif [ "$yn" == "N" ] || [ "$yn" == "n" ]; then
    echo "Oh, interrupt!"
else
    echo "I don't know your choice is"
fi

exit 0
