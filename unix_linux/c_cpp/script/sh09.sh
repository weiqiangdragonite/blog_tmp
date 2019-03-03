#!/bin/bash
#
# Program:
#   Check $1 is equal to "hello"
#
# History:
#   2005/08/28  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

if [ "$1" == "hello" ]; then
    echo "Hello, how are you?"
elif [ "$1" == "" ]; then
    echo "You MUST input parameter, ex> {$0 someword}"
else
    echo "The only parameter is 'hello', ex> {$0 hello}"
fi

exit 0
