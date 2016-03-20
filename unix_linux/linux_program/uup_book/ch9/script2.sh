#!/bin/bash
# script2: a real program with variables, input
#          and control flow


BOOK=$phonebook.data
echo find what name in phonebook
read NAME


if grep $NAME $BOK > /tmp/pb.tmp
then
	echo Enteries for $NAME
	cat /tmp/pb.tmp
else
	echo No enteries for $NAME
fi
rm /tmp/pb.tmp

