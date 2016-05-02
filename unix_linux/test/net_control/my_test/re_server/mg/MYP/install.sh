#!/bin/sh
gcc ping.c -o MYP -lmysqlclient -L/usr/local/lib/mysql -L/usr/lib -L/lib -I/usr/local/include
mv MYP /mg/MYP/
