#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket

def nonblocking_way():
    sock = socket.socket()
    sock.setblocking(False)
    try:
        sock.connect(("www.jobbole.com", 80))
    except BlockingIOError:
        #print("connect error")
        pass
    request = "GET / HTTP/1.0\r\nHost: www.jobbole.com\r\n\r\n"
    data = request.encode("ascii")
    while True:
        try:
            sock.send(data)
            break
        except OSError:
            #print("send error")
            pass

    response = b""
    while True:
        try:
            chunk = sock.recv(4096)
            while chunk:
                response += chunk
                chunk = sock.recv(4096)
            break
        except OSError:
            #print("recv error")
            pass
    return len(response)

def sync_way():
    res = []
    for i in range(10):
        res.append(nonblocking_way())
    return res

if __name__ == "__main__":
    print(sync_way())
