#!/bin/bash
#
# Program:
#   User input dir name, and find the permission of files
#
# History:
#   2005/08/29  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

#
read -p "Please input a directory: " dir
if [ "$dir" == "" -o ! -d "$dir" ]; then
    echo "The $dir is NOT exist."
    exit 1
fi

#
filelist=$(ls $dir)
for filename in $filelist
do
    perm=""
    test -r "$dir/$filename" && perm="$perm r"
    test -w "$dir/$filename" && perm="$perm w"
    test -x "$dir/$filename" && perm="$perm x"

    echo "The file $dir/$filename's permission is $perm"
done

exit 0
