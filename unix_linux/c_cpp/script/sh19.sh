#!/bin/bash
#
# Program:
#   Try to calculate 1+2+3...+${your_input}
#
# History:
#   2005/08/29  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

read -p "Please input a number, I will count for 1+2+3+...+your_input: " \
input

result=0
for ((i = 1; i <= $input; ++i))
do
    result=$(($result + $i))
done

echo "The result of '1+2+...+$input' is $result"

exit 0
