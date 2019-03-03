#!/bin/bash
#
# Program:
#   This script calculates '1+2+3+...+user_input'.
#
# By:
#   Dragonite
#   2013-7-21

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

while [ "$input" == "" ]
do
    read -p "Please input a number: " num
    input=$(echo $num | grep '[0-9]*')
done

result=0
for (( i = 1; i <= $input; ++i ))
do
    result=$(($result + $i))
done

echo "The result of '1+2+3+...+$input' is $result"

exit 0

exit 0
