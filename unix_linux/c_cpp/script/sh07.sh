#!/bin/bash
#
# Program:
#   This program shows the script name, parameters...
#
# History:
#   2009/02/17  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

echo "The script name is: $0"
echo "Total parameter number is: $#"

[ "$#" -lt 2 ] && echo "The number of parameter is less than 2. Stop \
here." && exit 0

echo "Your whole parameteris: '$@'"
echo "The 1st parameter is: $1"
echo "The 2nd parameter is: $2"

echo "all: '$*'"

exit 0
