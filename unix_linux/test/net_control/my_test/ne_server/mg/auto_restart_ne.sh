#!/bin/sh
/mg/yspro &
while true
do
  PROCESS_NUM=`ps -ef | grep "yspro" | grep -v "grep" | wc -l`
if [ $PROCESS_NUM -eq 0  ];
 then
  killall -9 yspro
  exec /mg/yspro &
 fi
 echo $PROCESS_NUM
 sleep 4
done
