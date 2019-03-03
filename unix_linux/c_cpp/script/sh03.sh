#!/bin/bash
#
# Program:
#   This program create the file, which named by user's input
#   and date command.
#
# Author:
#   Dragonite
#
# Date:
#   2013-7-21

# setting path
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

# 
read -p "Please input your filename: " fileuser

#
filename=${fileuser:-"filename"}

#
date1=$(date --date='2 days ago' +%Y%m%d)
date2=$(date --date='1 days ago' +%Y%m%d)
date3=$(date +%Y%m%d)

#
file1=${filename}${date1}
file2=${filename}${date2}
file3=${filename}${date3}

#
touch "$file1"
touch "$file2"
touch "$file3"

exit 0
