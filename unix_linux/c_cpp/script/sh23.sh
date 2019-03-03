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

dir="/home/dragonite/test/logical"
if [ -e $dir ]; then
    if [ -f $dir ]; then
        rm $dir
        mkdir $dir
    else
        rm -rf $dir
    fi
else
    touch $dir
fi

exit 0
