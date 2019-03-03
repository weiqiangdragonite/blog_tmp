#!/bin/bash
#
# Program:
#   Use loop to calculate "1+2+3+...+100" result
#
# History:
#   2005/08/29  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

result=0
num=0

while [ "$num" != "100" ]
do
    #num=$(($num+1))
    let ++num
    result=$(($result+$num))
done

echo "The result of '1+2+3+...+100' is $result"

exit 0
