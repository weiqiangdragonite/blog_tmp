#!/bin/bash
#
# Program:
#   Use for loop to print 3 animals
#
# History:
#   2005/08/29  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

for animal in dog cat elephant
do
    echo "I am $animal"
done

exit 0
