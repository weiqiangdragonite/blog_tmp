#!/bin/bash
#
# Program:
#   This script calculates how many days befor your birthday.
#
# By:
#   Dragonite
#   2013-7-21

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

while [ "$born_date" == "" ]
do
    read -p "Please input your born date (YYYYMMDD): " date
    born_date=$(echo $date | grep '[0-9]\{4\}')
done

born_date=$(date +%Y)${born_date:4:4}
#echo "born_date: $born_date"

#
declare -i birthday=$(date --date="$born_date" +%s)
declare -i today=$(date +%s)
declare -i rest=$(($birthday - $today))
declare -i day=$(($rest/60/60/24))

#
if [ "$day" -eq "0" ]; then
    echo "Today is your birthday! HAPPY BIRTHDAY!!!"
elif [ "$day" -lt "0" ]; then
    echo "Your birthday is after $((365+$day)) days!"
else
    echo "Your birthday is after $day days!"
fi

exit 0
