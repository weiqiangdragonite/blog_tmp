#!/bin/bash
#
# Program:
#   User inputs 2 integer numbers, program will cross these two numbers.
#
# History:
#   2005/08/23  VBird   First Release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

#
echo -e "You should input 2 numbers, I will cross them!\n"

#
read -p "first number: " num1
read -p "second number: " num2

#
total=$(($num1*$num2))

#
echo "The result is: $total"

exit 0
