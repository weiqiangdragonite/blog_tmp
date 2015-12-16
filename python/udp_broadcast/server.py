#!/usr/bin/env python
# -*- coding: utf-8 -*-

# this is the udp broadcast server

import socket, traceback


host = '' # Bind to all interfaces
port = 12340

print "python UDP multi case server test"


s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
s.bind((host, port))

for i in range(1,100):
    try:
        s.sendto("[s]server here",('255.255.255.255', port))
        message, address = s.recvfrom(1024)
        print "Got data from", address,":",message 
        # Acknowledge it.
        s.sendto("I am here", address)
    except (KeyboardInterrupt, SystemExit):
        raise
    except:
        traceback.print_exc()

