#!/bin/bash
#
# Program:
#   Using netstat and grep to detect www, ssh, ftp, and mail services.
#
# History:
#   2005/08/28  VBird   First release

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

#
echo "Now, I will detect your linux server's services!"
echo -e "The www, ftp, ssh, and mial services will be detect!\n"

# check the port 80 - www
testing=$(netstat -tuln | grep ":80")
if [ "$testing" != "" ]; then
    echo "www is running in your system."
else
    echo "www is not running in your system."
fi

# check the port 22 - ssh
testing=$(netstat -tuln | grep ":22")
if [ "$testing" != "" ]; then
    echo "ssh is running in your system."
else
    echo "ssh is not running in your system."
fi

# check the port 21 - ftp
testing=$(netstat -tuln | grep ":21")
if [ "$testing" != "" ]; then
    echo "ftp is running in your system."
else
    echo "ftp is not running in your system."
fi

# check the port 25 - mail
testing=$(netstat -tuln | grep ":25")
if [ "$testing" != "" ]; then
    echo "mail is running in your system."
else
    echo "mail is not running in your system."
fi

exit 0
