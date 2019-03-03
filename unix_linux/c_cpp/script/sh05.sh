#!/bin/bash
#
# Program:
#   User input a filename, program will check the following:
#   1.) exist? 2.) file/directory? 3.) permissions
#
# History:
#   2005/08/25  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

#
echo -e "please input a filename, I will check the filename's type and \
permission."

read -p "input a filename: " filename

# test the input string
test -z $filename && echo "You MUST input a filename." && exit 0

# check the file if is exist
test ! -e $filename && echo "The $filename does not exist." && exit 0

# check the file type
test -f $filename && filetype="regular file"
test -d $filename && filetype="directory"

# check the file permission
test -r $filename && perm="r"
test -w $filename && perm="$perm w"
test -x $filename && perm="$perm x"

# print the msg
echo "The $filename is a $filetype"
echo "And the permission is: $perm"

exit 0
