#!/usr/bin/env python
# -*- coding: utf-8 -*-

# http://ip.taobao.com/instructions.php

import socket


#
common_headers = \
    "Host: ip.taobao.com\r\n" + \
    "Connection: Keep-Alive\r\n" + \
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n" + \
    "User-Agent: Mozilla/5.0 (X11; Linux) AppleWebKit/538.1 (KHTML, like Gecko) Chrome/18.0.1025.133 Safari/538.1 Midori/0.5\r\n" + \
    "Accept-Language: en-us;q=0.750\r\n"

# 通过 GET
get_headers = \
    "GET /service/getIpInfo.php?ip=myip HTTP/1.1\r\n" + \
    common_headers + \
    "\r\n"

# 通过 POST
post_headers = \
    "POST /service/getIpInfo2.php HTTP/1.1\r\n" + \
    common_headers + \
    "Content-Length: 7\r\n" + \
    "\r\n" + \
    "ip=myip";


if __name__ == "__main__":
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("ip.taobao.com", 80))
    s.send(get_headers)

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