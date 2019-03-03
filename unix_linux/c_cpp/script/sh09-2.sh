#!/bin/bash
#
# Program:
#   Show "Hello" form $1... by using case ... esac
#
# History:
#   2005/08/29  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

case $1 in
    "hello")
        echo "Hello, how are you?"
        ;;
    "")
        echo "You MUST input parameters, ex> {$0 someword}"
        ;;
    *)
        echo "Usage $0 {hello}"
        ;;
esac

exit 0
