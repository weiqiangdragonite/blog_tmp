#!/usr/bin/env python
# -*- coding: utf-8 -*-

#this is the udp broadcast client

import socket, traceback


port = 12340

print "python UDP multi case client test"

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

data = "Data from pc"
s.bind(("",port))
for i in range(1,100):
    try:
        data,address=s.recvfrom(1024)
        print "cli get data form", address, ":", data
        s.sendto("[c]client ack", ('255.255.255.255', port))
        #data,address = s.recvfrom(1024)
        #print "received %r from %r" % (data, address)
    except (KeyboardInterrupt, SystemExit):
        raise
    except:
        traceback.print_exc()
    
print "test finsh"