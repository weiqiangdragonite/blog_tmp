#!/usr/bin/env python
# -*- coding: utf-8 -*-

# http://ip.taobao.com/instructions.php

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("ip.taobao.com", 80))

# 通过 GET

# 通过 POST
headers = \
    "POST /service/getIpInfo2.php HTTP/1.1\r\n" + \
    "Host: ip.taobao.com\r\n" + \
    "Connection: Keep-Alive\r\n" + \
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n" + \
    "User-Agent: Mozilla/5.0 (X11; Linux) AppleWebKit/538.1 (KHTML, like Gecko) Chrome/18.0.1025.133 Safari/538.1 Midori/0.5\r\n" + \
    "Accept-Language: en-us;q=0.750\r\n" + \
    "Content-Type: application/x-www-form-urlencoded\r\n" + \
    "Content-Length: 7\r\n" + \
    "\r\n" + \
    "ip=myip";

s.send(headers)

buffer = []
while True:
    d = s.recv(1024)
    if d:
        buffer.append(d)
    else:
        break
data = ''.join(buffer)
s.close()

print data