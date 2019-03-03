#!/bin/bash
#
# Program:
#   This program shows the effect of shift function.
#
# History:
#   2009/02/17  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

echo "Total parameter number is: $#"
echo "Your whole parameteris: '$@'"

# first shift
shift
echo "Total parameter number is: $#"
echo "Your whole parameteris: '$@'"

# second shift
shift 3
echo "Total parameter number is: $#"
echo "Your whole parameteris: '$@'"

exit 0
