#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket

def test_socket_timeout():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print("Default socket timeout: %s" % s.gettimeout())
    s.settimeout(100)   # seconds
    print("Current socket timeout: %s" % s.gettimeout())


if __name__ == "__main__":
    test_socket_timeout()



