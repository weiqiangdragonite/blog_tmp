#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket

def blocking_way():
    sock = socket.socket()
    # blocking
    sock.connect(("www.jobbole.com", 80))
    request = "GET / HTTP/1.0\r\nHost: www.jobbole.com\r\n\r\n"
    sock.send(request.encode("ascii"))
    response = b""
    chunk = sock.recv(4096)
    while chunk:
        response += chunk
        # blocking
        chunk = sock.recv(4096)
    return len(response)

def sync_way():
    res = []
    for i in range(10):
        res.append(blocking_way())
    return res

if __name__ == "__main__":
    print(sync_way())
